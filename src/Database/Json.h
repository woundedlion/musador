#pragma once

#include <vector>
#include <map>
#include <queue>
#include <cstdint>

#include <boost/serialization/nvp.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/version.hpp>
#include <boost/spirit/home/support/detail/hold_any.hpp>

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
			void write_value(T *t)
			{
				if (t) {
					write_value(*t);
				}
				else {
					writer.Null();
				}
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

			InputArchive(std::istream& in) :
				in(in)
			{}

			template <typename T>
			InputArchive& operator >>(T& t)
			{
				return (*this) & t;
			}

			template <typename T>
			InputArchive& operator &(T& t)
			{
				boost::serialization::serialize_adl(
					*this,
					t,
					boost::serialization::version<T>::value);

				rapidjson::Reader parser;
				parser.Parse<rapidjson::kParseValidateEncodingFlag>(in, SAXHandler(out));
				check_underrun();

				return *this;
			}

			template <typename T>
			InputArchive& operator &(const boost::serialization::nvp<T>& t)
			{
				out.push(make_tv(t.value()));
				return *this;
			}

		private:

			typedef std::queue<TypeValue> TypeValueQueue;

			class SAXHandler
			{
			public:

				typedef char Ch;

				SAXHandler(TypeValueQueue& out) : out(out) {}

				void Null() {}
				void Bool(bool v) { assign_from(v); }
				void Int(int v) { assign_from(v); }
				void Uint(unsigned v) { assign_from(v); }
				void Int64(int64_t v) { assign_from(v); }
				void Uint64(uint64_t v) { assign_from(v); }
				void Double(double v) { assign_from(v); }
				void String(const char* str, size_t length, bool copy) { 
					assign_from(std::string(str, str + length));
				}

				void StartObject() {}
				void EndObject(size_t memberCount) {}
				void StartArray() {}
				void EndArray(size_t elementCount) {}

			private:

				template <typename T>
				void assign_from(const T& src)
				{
					if (++value_count % 2 == 0) {
						check_overrun();
						assign_to(out.front(), src);
						out.pop();
					}
				}

				inline void check_overrun()
				{
					if (out.empty()) {
						throw std::runtime_error("Parse error: source data too large");
					}
				}

				TypeValueQueue& out;
				size_t value_count = 0;
			};

			template <typename T>
			inline TypeValue make_tv(T& dst)
			{
				return TypeValue(Traits<T>::type_id, &dst);
			}

			inline void check_underrun()
			{
				if (!out.empty()) {
					throw std::runtime_error("Parse error: source data incomplete");
				}
			}

			InputStreamWrapper in;
			TypeValueQueue out;
		};
	}
}