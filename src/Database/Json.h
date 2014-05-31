#pragma once

#include <vector>
#include <map>
#include <queue>
#include <stack>
#include <cstdint>

#include <boost/serialization/nvp.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/version.hpp>

// TODO: remove Util dependency
#include "Utilities/Util.h"

#include "JsonTypes.h"

#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/reader.h"

namespace storm {
	namespace json {

		class InputStreamWrapper
		{
		public:

			typedef char Ch;
			InputStreamWrapper(std::istream& s) : s(&s) {}

			char Peek() const { return s->peek(); }
			char Take() const { return s->get(); }
			size_t Tell() { return s->tellg(); }

			Ch* PutBegin() { return 0; }
			void Put(Ch) { }
			void Flush() {  }
			size_t PutEnd(Ch*) { return 0;  }

		private:

			std::istream *s;
		};

		class OutputArchive
		{
		public:

			typedef boost::mpl::bool_<true> is_saving;
			typedef boost::mpl::bool_<false> is_loading;

			OutputArchive(std::ostream& out) :
				out(out),
				writer(buf)
			{}

			template <typename T>
			OutputArchive& operator <<(T& t)
			{
				return (*this) & t;
			}

			template <typename T>
			OutputArchive& operator &(T& t)
			{
				writer.StartObject();
				boost::serialization::serialize_adl(
					*this,
					t,
					boost::serialization::version<T>::value);
				writer.EndObject();
				// TODO: Remove copy by wrapping std::ostream instead of using rapidjson::StringBuffer
				out << buf.GetString() << std::endl;
				buf.Clear();
				return *this;
			}

			template <typename T>
			OutputArchive& operator &(const boost::serialization::nvp<T>& t)
			{
				writer.String(t.name());
				write_value(t.value());
				return *this;
			}

		private:

			template <typename T> 
			void write_value(T& t)
			{
				(*this) & t;
			}

			template <typename T>
			void write_value(std::vector<T>& v)
			{
				writer.StartArray();
				for (auto i : v) {
					write_value(i);
				}
				writer.EndArray();
			}

			template <typename T>
			void write_value(std::map<std::string, T>& v)
			{
				writer.StartObject();
				for (auto i : v) {
					writer.String(i.first.c_str());
					write_value(i.second);
				}
				writer.EndObject();
			}

			inline void write_value(std::string& v) { write_value(v.c_str()); }
			inline void write_value(std::wstring& v) { write_value(v.c_str()); }
			inline void write_value(char v) { write_value(std::string(1, v)); }
			inline void write_value(wchar_t v) { write_value(std::wstring(1, v)); }
			inline void write_value(bool v) { writer.Bool(v); }
			inline void write_value(const char *v) { writer.String(v); }
			inline void write_value(const wchar_t *v) { writer.String(Util::unicodeToUtf8(v).c_str()); }
			inline void write_value(uint32_t v) { writer.Uint(v); }
			inline void write_value(uint64_t v) { writer.Uint64(v); }
			inline void write_value(int v) { writer.Int(v); }
			inline void write_value(int64_t v) { writer.Int64(v); }
			inline void write_value(float v) { writer.Double(v, std::numeric_limits<float>::max_digits10); }
			inline void write_value(double v) { writer.Double(v, std::numeric_limits<double>::max_digits10); }

			std::ostream& out;
			rapidjson::StringBuffer buf;
			rapidjson::PrettyWriter<decltype(buf)> writer;
		};

		class InputArchive
		{
		public:

			typedef boost::mpl::bool_<false> is_saving;
			typedef boost::mpl::bool_<true> is_loading;

			InputArchive(std::istream& is) :
				in(is)
			{
				rapidjson::Reader parser;
				parser.Parse<rapidjson::kParseValidateEncodingFlag>(in, SAXHandler(out));
				cur_val = out.cbegin();
			}

			template <typename T>
			InputArchive& operator >>(T& t)
			{
				return (*this) & t;
			}

			template <typename T>
			InputArchive& operator &(T& t)
			{
				if (cur_val == out.cend()) {
					throw std::runtime_error("Parse error: Source too short");
				}
				++cur_val;

				boost::serialization::serialize_adl(
					*this,
					t,
					boost::serialization::version<T>::value);

				if (cur_val != out.cend()) {
					throw std::runtime_error("Parse error: Source too long");
				}

				return *this;
			}

			template <typename T>
			InputArchive& operator &(const boost::serialization::nvp<T>& dst)
			{
				if (cur_val == out.cend()) {
					throw std::runtime_error("Parse error: Source too short");
				}

				boost::apply_visitor(check_name(dst.name()), *cur_val++);
				read_value(dst.value());
				return *this;
			}

		private:

			class SAXHandler
			{
			public:

				typedef char Ch;

				SAXHandler(std::vector<Value>& out) :
					state({ State::Initial }),
				    out(out)
				{}
				~SAXHandler() {}

				void Null() { parse(nullptr); }
				void Bool(bool v) { parse(v); }
				void Int(int v) { parse(v); }
				void Uint(unsigned v) { parse(v); }
				void Int64(int64_t v) { parse(v); }
				void Uint64(uint64_t v) { parse(v); }
				void Double(double v) { parse(v); }
				void String(const char* str, size_t length, bool copy) { parse(str, length); }

				void StartObject()
				{ 
					switch (state.top()) {
					case State::InObject:
						throw std::runtime_error("Parse Error: Unnamed nested object");
					case State::Initial:
						assert(backrefs.empty());
					case State::InArray:
					case State::InValue:
						out.emplace_back(Object());
						backrefs.push(out.size() - 1);
						break;
					default:
						assert(false);
					}
					state.push(State::InObject);
				}

				void EndObject(size_t count) { 
					boost::apply_visitor(close_object(count), out[backrefs.top()]);
					backrefs.pop();
					state.pop(); 
				}

				void StartArray() {
					switch (state.top()) {
					case State::Initial:
						assert(backrefs.empty());
					case State::InObject:
					case State::InArray:
					case State::InValue:
						out.emplace_back(Array());
						backrefs.push(out.size() - 1);
						break;
					default:
						assert(false);
					}
					state.push(State::InArray);
				}

				void EndArray(size_t count) {
					boost::apply_visitor(close_array(count), out[backrefs.top()]);
					backrefs.pop();
					state.pop();
				}

			private:

				enum class State
				{
					Initial, InObject, InArray, InValue
				};

				typedef std::stack<State> States;
				typedef std::stack<size_t> BackRefs;
				typedef std::vector<Value> Values;

				template <typename T>
				void parse(const T& t)
				{
					switch (state.top()) {
					case State::Initial:
						throw std::runtime_error("Parse Error: Unnamed value at root");
					case State::InObject:
						throw std::runtime_error("Parse Error: Unnamed member");
					case State::InValue:
						parse_value(t);
						state.pop();
						break;
					case State::InArray :
						parse_value(t);
						break;
					default:
						assert(false);
					}
				}

				void parse(const char* str, size_t length)
				{
					switch (state.top()) {
					case State::Initial:
					case State::InObject:
						parse_value(str, length);
						state.push(State::InValue);
						break;
					case State::InValue:
						parse_value(str, length);
						state.pop();
						break;
					case State::InArray:
						parse_value(str, length);
						break;
					default:
						assert(false);
					}
				}
				
				template <typename T>
				void parse_value(const T& value)
				{
					out.push_back(value);
				}

				void parse_value(const char* str, size_t length)
				{
					out.emplace_back(std::string(str, length));
				}

				Values& out;
				States state;
				BackRefs backrefs;
			};
			
			template <typename T, typename std::enable_if<std::is_class<T>::value>::type* = nullptr>
			void read_value(T& t)
			{
				*this & t;
			}

			template <typename T, typename std::enable_if<std::is_reference<T>::value>::type* = nullptr>
			void read_value(T& t)
			{
				read_value(*t);
			}

			template <typename T, typename std::enable_if<std::is_scalar<T>::value>::type* = nullptr>
			void read_value(T& t)
			{
				boost::apply_visitor(assign_to<T>(t), *cur_val++);
			}

			void read_value(std::string& s)
			{
				boost::apply_visitor(assign_to<decltype(s)>(s), *cur_val++);
			}

			void read_value(std::wstring& s)
			{
				boost::apply_visitor(assign_to<decltype(s)>(s), *cur_val++);
			}

			template <typename T>
			void read_value(std::vector<T>& v)
			{
				const Array& a = boost::get<Array>(*cur_val++);
				std::vector<T>(a.items).swap(v);
				for (auto& i : v) {
					read_value(i);
				}
			}

			template <typename T>
			void read_value(std::map<std::string, T>& map)
			{
				const Object& o = boost::get<Object>(*cur_val++);
				map.clear();
				for (size_t i = 0; i < o.items; ++i)
				{
					insert_item<T> inserter(map);
					boost::apply_visitor(inserter, *cur_val++);
					read_value(inserter.last_value());
				}
			}

			InputStreamWrapper in;
			std::vector<Value> out;
			std::vector<Value>::const_iterator cur_val;
		};
	}
}