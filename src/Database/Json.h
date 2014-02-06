#pragma once

#include <vector>
#include <map>
#include <queue>
#include <cstdint>

#include <boost/serialization/nvp.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/version.hpp>

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
			InputStreamWrapper(std::istream& s) : s(s) {}
			
			char Peek() const { return s.peek(); }
			char Take() const { return s.get(); }
			size_t Tell() { return s.tellg(); }

		private:

			std::istream& s;
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

		class SAXHandler
		{
		public:

			typedef char Ch;

			SAXHandler(std::queue<uintptr_t>& out) : out(out) {}
			void Null() { out.front}
			void Bool(bool b);
			void Int(int i);
			void Uint(unsigned i);
			void Int64(int64_t i);
			void Uint64(uint64_t i);
			void Double(double d);
			void String(const Ch* str, SizeType length, bool copy);
			void StartObject();
			void EndObject(SizeType memberCount);
			void StartArray();
			void EndArray(SizeType elementCount);

		private:

			std::queue<uintptr_t>& out;
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

				rapidjson::Reader reader;
				r.Parse(s, handler)

				return *this;
			}

			template <typename T>
			InputArchive& operator &(boost::serialization::nvp<T>& t)
			{
				out.push(&t.value());
			}

		private:

			InputStreamWrapper in;
			std::queue<uintptr_t> out;
		};

	}
}