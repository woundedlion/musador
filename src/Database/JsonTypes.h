#pragma once

#include <string>
#include <cstdint>
#include <type_traits>

namespace storm {
	namespace json {

		enum class Type
		{
			BOOL,
			STRING, WSTRING, CHAR, WCHAR, CHAR_P, WCHAR_P,
			UINT32, UINT64, INT, INT64, FLOAT, DOUBLE,
			NUM_TYPES
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

		struct TypeValue
		{
			TypeValue(Type type, void *value) : type(type), value(value) {}

			Type type;
			void *value;
		};

		template <typename T>
		void assign_to(const TypeValue& dst, const T& src)
		{
			switch (dst.type) {
			case Type::BOOL:
				assign(*reinterpret_cast<bool *>(dst.value), src);
				break;
			case Type::UINT32:
				assign(*reinterpret_cast<uint32_t *>(dst.value), src);
				break;
			case Type::UINT64:
				assign(*reinterpret_cast<uint64_t *>(dst.value), src);
				break;
			case Type::INT:
				assign(*reinterpret_cast<int *>(dst.value), src);
				break;
			case Type::INT64:
				assign(*reinterpret_cast<int64_t *>(dst.value), src);
				break;
			case Type::FLOAT:
				assign(*reinterpret_cast<float *>(dst.value), src);
				break;
			case Type::DOUBLE:
				assign(*reinterpret_cast<double *>(dst.value), src);
				break;
			default:
				throw std::runtime_error("Parse Error: Unsupported type");
			}
		}

		template <typename Dst, typename Src>
		void assign(Dst& dst, const Src& src, typename std::enable_if<std::is_same<Dst, Src>::value >::type* = 0)
		{
			dst = src;
		}

		template <typename Dst, typename Src>
		void assign(Dst& dst, const Src& src, typename std::enable_if<!std::is_same<Dst, Src>::value >::type* = 0)
		{
				throw std::runtime_error("Parse Error: Type mismatch!");
		}

		void assign(int& dst, uint32_t src) { dst = static_cast<int>(src); }

		void assign(int64_t& dst, uint32_t src) { dst = static_cast<int>(src); }
		void assign(int64_t& dst, int src) { dst = static_cast<int>(src); }

		void assign(uint64_t& dst, uint32_t src) { dst = src; }

		void assign(float& dst, double src) { dst = static_cast<float>(src); }
		void assign(float& dst, int src) { dst = static_cast<float>(src); }
		void assign(float& dst, uint32_t src) { dst = static_cast<float>(src); }

		void assign(double& dst, int src) { dst = static_cast<double>(src); }
		void assign(double& dst, int64_t src) { dst = static_cast<double>(src); }
		void assign(double& dst, uint32_t src) { dst = static_cast<double>(src); }
		void assign(double& dst, uint64_t src) { dst = static_cast<double>(src); }
	}
}
