#pragma once

#include <boost/serialization/nvp.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/version.hpp>
// TODO: remove Util dependency
#include "Utilities/Util.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h"

namespace storm {
	namespace json {

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
			void write_value(T& t, 
				typename std::enable_if<
				std::is_class<typename std::remove_reference<T>::type>::value
				&& !std::is_base_of<std::basic_string<char>, T>::value
				&& !std::is_base_of<std::basic_string<wchar_t>, T>::value>::type * = 0)
			{
				(*this) & t;
			}

			template <class T>
			void write_value(T& t, 
				typename std::enable_if<
				std::is_base_of<std::basic_string<typename T::value_type>, T>::value>::type * = 0)
			{
				write_value(t.c_str());
			}

			template <typename T>
			void write_value(T& t, typename std::enable_if<
				std::is_pointer<T>::value>::type * = 0)
			{
				if (t) {
					(*this) & *t;
				}
				else {
					writer.Null();
				}
			}
			
			void write_value(bool v) { writer.Bool(v); }
			void write_value(const char *v) { writer.String(v); }
			void write_value(const wchar_t *v) { writer.String(Util::unicodeToUtf8(v).c_str()); }
			void write_value(uint32_t v) { writer.Uint(v); }
			void write_value(uint64_t v) { writer.Uint64(v); }
			void write_value(int v) { writer.Int(v); }
			void write_value(int64_t v) { writer.Int64(v); }
			void write_value(float v) { writer.Double(v); }
			void write_value(double v) { writer.Double(v); }

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

			}

			template <typename T>
			InputArchive& operator &(const boost::serialization::nvp<T>& t)
			{

			}

		private:

			std::istream& in;
		};

	}
}