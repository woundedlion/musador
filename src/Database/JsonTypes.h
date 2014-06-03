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

		struct Object { size_t items = 0; };
		struct Array { size_t items = 0; };
		typedef boost::variant<nullptr_t, bool, int, unsigned int, int64_t, uint64_t, double, std::string, Object, Array> Value;

		// Visitors

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

		struct check_name : public boost::static_visitor<>
		{
			check_name(const char *name) : name(name) {}

			void operator()(const std::string& n) const
			{
				if (n != name) {
					throw std::runtime_error("Parse Error: Member name mismatch");
				}
			}
			
			template <typename T>
			void operator()(T& t) const
			{
				throw std::runtime_error("Parse Error: Expected member name");
			}

			const char *name;
		};

		template <typename Dst>
		struct assign_to : public boost::static_visitor<>
		{
			assign_to(Dst& dst) : dst(dst) {}
			
			template <typename Src>
			void operator()(const Src& src) const
			{
				assign(dst, src);
			}

			Dst& dst;
		};

		template <typename V>
		class insert_item : public boost::static_visitor<>
		{
		public:
			insert_item(std::map<std::string, V>& dst) : dst(dst), inserted({ dst.end(), false }) {}

			template <typename N>
			void operator()(const N&) const
			{
				throw std::runtime_error("Parse Error: Expected member name");
			}

			void operator()(const std::string& name) const
			{
				inserted = dst.emplace(std::make_pair(name, V()));
				if (!inserted.second) {
					throw std::runtime_error("Parse Error: Duplicate member name");
				}
			}

			V& last_value()
			{
				assert(inserted.second);
				return inserted.first->second;
			}

		private:

			std::map<std::string, V>& dst;
			mutable std::pair<typename std::map<std::string, V>::iterator, bool> inserted;
		};

		// Conversion helpers

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

		static inline void assign(int16_t& dst, int src) { dst = static_cast<int16_t>(src); }
		static inline void assign(uint16_t& dst, uint32_t src) { dst = static_cast<uint16_t>(src); }
		static inline void assign(int& dst, uint32_t src) { dst = static_cast<int>(src); }
		static inline void assign(int64_t& dst, uint32_t src) { dst = static_cast<int64_t>(src); }
		static inline void assign(int64_t& dst, uint64_t src) { dst = static_cast<int64_t>(src); }
		static inline void assign(int64_t& dst, int src) { dst = src; }
		static inline void assign(uint64_t& dst, uint32_t src) { dst = src; }
		static inline void assign(float& dst, double src) { dst = static_cast<float>(src); }
		static inline void assign(float& dst, int src) { dst = static_cast<float>(src); }
		static inline void assign(float& dst, uint32_t src) { dst = static_cast<float>(src); }
		static inline void assign(double& dst, int src) { dst = static_cast<double>(src); }
		static inline void assign(double& dst, int64_t src) { dst = static_cast<double>(src); }
		static inline void assign(double& dst, uint32_t src) { dst = static_cast<double>(src); }
		static inline void assign(double& dst, uint64_t src) { dst = static_cast<double>(src); }
		static inline void assign(std::wstring& dst, const std::string& src) { dst = Util::utf8ToUnicode(src); }
		static inline void assign(char& dst, const std::string& src) { dst = src.empty() ? '\0' : src[0]; }
		static inline void assign(wchar_t& dst, const std::string& src) { dst = (src.empty() ? L'\0' : Util::utf8ToUnicode(src[0])[0]); }
	}
}
