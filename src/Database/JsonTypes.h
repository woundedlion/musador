#pragma once

#include <string>
#include <cstdint>
#include <cstddef>
#include <stdexcept>
#include <type_traits>
#include <boost/variant.hpp>

#include "Utilities/Util.h"

namespace storm {
	namespace json {

		struct Object
		{
			size_t items = 0;
		};

		struct Array
		{
			size_t items = 0;
		};

		struct close_object : public boost::static_visitor<>
		{
			close_object(size_t items) : items(items) {}

			void operator()(Object& obj) const
			{
				obj.items = items;
			}

			template <typename T>
			void operator()(T& t) const
			{
				throw std::runtime_error("Parse Error: Unmatched object close");
			}

			const size_t items;
		};

		struct close_array : public boost::static_visitor<>
		{
			close_array(size_t items) : items(items) {}

			void operator()(Array& arr) const
			{
				arr.items = items;
			}
			
			template <typename T>
			void operator()(T& t) const
			{
				throw std::runtime_error("Parse Error: Unmatched array close");
			}

			const size_t items;
		};

		typedef boost::variant<nullptr_t, bool, int, unsigned int, int64_t, uint64_t, double, std::string, Object, Array> Value;

		template <typename Dst, typename Src>
		void assign(Dst& dst, const Src& src, typename std::enable_if<std::is_same<Dst, Src>::value >::type* = 0)
		{
			dst = src;
		}

		template <typename Dst, typename Src>
		void assign(Dst& dst, const Src& src, typename std::enable_if<!std::is_same<Dst, Src>::value >::type* = 0)
		{
			throw std::runtime_error("Parse Error: Type mismatch");
		}

		void assign(int& dst, uint32_t src) { dst = static_cast<int>(src); }
		void assign(int64_t& dst, uint32_t src) { dst = static_cast<int64_t>(src); }
		void assign(int64_t& dst, uint64_t src) { dst = static_cast<int64_t>(src); }
		void assign(int64_t& dst, int src) { dst = src; }
		void assign(uint64_t& dst, uint32_t src) { dst = src; }
		void assign(float& dst, double src) { dst = static_cast<float>(src); }
		void assign(float& dst, int src) { dst = static_cast<float>(src); }
		void assign(float& dst, uint32_t src) { dst = static_cast<float>(src); }
		void assign(double& dst, int src) { dst = static_cast<double>(src); }
		void assign(double& dst, int64_t src) { dst = static_cast<double>(src); }
		void assign(double& dst, uint32_t src) { dst = static_cast<double>(src); }
		void assign(double& dst, uint64_t src) { dst = static_cast<double>(src); }
		void assign(std::wstring& dst, const std::string& src) { dst = Util::utf8ToUnicode(src);  }
		void assign(char& dst, const std::string& src) { dst = src.empty() ? '\0' : src[0]; }
		void assign(wchar_t& dst, const std::string& src) { dst = (src.empty() ? L'\0' : Util::utf8ToUnicode(src[0])[0]); }
	}
}
