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
			inline void write_value(float v) { writer.Double(v); }
			inline void write_value(double v) { writer.Double(v); }

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
				parser.Parse<rapidjson::kParseValidateEncodingFlag>(in, SAXHandler(readers));
				check_underrun();

				return *this;
			}

			template <typename T>
			InputArchive& operator &(const boost::serialization::nvp<T>& t)
			{
				readers.push(read_into(t.value()));
				return *this;
			}

		private:

			enum class Type
			{
				BOOL,
				STRING, WSTRING, CHAR, WCHAR, CHAR_P, WCHAR_P, 
				UINT32, UINT64, INT, INT64, FLOAT, DOUBLE
			};

			template <typename T> struct Traits;
			template <> struct Traits<bool> { static const Type type_id = Type::BOOL; };

			template <> struct Traits<std::string> { static const Type type_id = Type::STRING; };
			template <> struct Traits<std::wstring> { static const Type type_id = Type::WSTRING; };
			template <> struct Traits<char> { static const Type type_id = Type::CHAR; };
			template <> struct Traits<wchar_t> { static const Type type_id = Type::WCHAR; };
			template <> struct Traits<char *> { static const Type type_id = Type::CHAR_P; };
			template <> struct Traits<wchar_t *> { static const Type type_id = Type::WCHAR_P; };
			
			template <> struct Traits<uint32_t> { static const Type type_id = Type::UINT32; };
			template <> struct Traits<uint64_t> { static const Type type_id = Type::UINT64; };
			template <> struct Traits<int> { static const Type type_id = Type::INT; };
			template <> struct Traits<int64_t> { static const Type type_id = Type::INT64; };
			template <> struct Traits<float> { static const Type type_id = Type::FLOAT; };
			template <> struct Traits<double> { static const Type type_id = Type::DOUBLE; };

			typedef std::vector<Type> TypeList;
			typedef std::function<void (void *)> ReadFunc;
			typedef std::pair<Type, ReadFunc> TypedReader;
			typedef std::queue<TypedReader> ReaderQueue;

			class SAXHandler
			{
			public:

				typedef char Ch;

				SAXHandler(ReaderQueue& readers) : readers(readers) {}

				void Null() {}
				void Bool(bool v) { copy({ Type::BOOL }, &v); }
				void Int(int v) { copy({ Type::INT, Type::INT64 }, &v); }
				void Uint(unsigned v) { copy({ Type::UINT32, Type::UINT64, Type::INT, Type::INT64, Type::FLOAT, Type::DOUBLE }, &v); }
				void Int64(int64_t v) { copy({ Type::INT64 }, &v); }
				void Uint64(uint64_t v) { copy({ Type::UINT64 }, &v); }
				void Double(double v) { copy({ Type::FLOAT, Type::DOUBLE}, &v); }
				void String(const char* str, size_t length, bool copy) {}

				void StartObject() {}
				void EndObject(size_t memberCount) {}
				void StartArray() {}
				void EndArray(size_t elementCount) {}

			private:

				void copy(const TypeList& valid_types, void *src)
				{
					check_overrun();
					check_type(valid_types);
					readers.front().second(src);
					readers.pop();
				}

				inline void check_overrun()
				{
					if (readers.empty()) {
						throw std::runtime_error("Parse error: source data too large");
					}
				}

				void check_type(const TypeList& valid_types)
				{
					if (std::find(valid_types.begin(), valid_types.end(),
						readers.front().first) == valid_types.end())
					{
						throw std::runtime_error("Parse Error: type mismatch");
					}
				}

				ReaderQueue& readers;
			};

			inline void check_underrun()
			{
				if (!readers.empty()) {
					throw std::runtime_error("Parse error: source data incomplete");
				}
			}

			template <typename T>
			inline TypedReader read_into(T& dst)
			{
				return std::make_pair(Traits<T>::type_id, [&](void *src)
				{
					dst = *reinterpret_cast<std::remove_reference<decltype(dst)>::type *>(src);
				});
			}

			inline TypedReader read_into(std::string& v) { }
			inline TypedReader read_into(std::wstring& v) { }
			inline TypedReader read_into(char& v) {  }
			inline TypedReader read_into(wchar_t& v) { }
			inline TypedReader read_into(char *& v) { }
			inline TypedReader read_into(wchar_t *& v) { }

			InputStreamWrapper in;
			ReaderQueue readers;
		};
	}
}