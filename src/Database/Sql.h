#pragma once

#include <sstream>
#include "boost/format.hpp"
#include "Utilities/Util.h"

namespace storm
{
	namespace sql
	{
		template <typename Entity>
		struct create {};

		template <typename Entity>
		struct drop {};

		template <typename T>
		inline std::wstring 
		quote(const T& t)
		{
			return std::to_wstring(t);
		};

		template <>
		inline std::wstring
		quote(const std::string& raw)
		{
			return (boost::wformat(L"'%1%'") % Util::utf8ToUnicode(raw)).str();
		}

		template <>
		inline std::wstring
		quote(const std::wstring& raw)
		{
			return (boost::wformat(L"'%1%'") % raw).str();
		}

		inline std::wstring
		quote(const wchar_t *raw)
		{
			return (boost::wformat(L"'%1%'") % raw).str();
		}

		inline std::wstring
		quote(const char *raw)
		{
			return (boost::wformat(L"'%1%'") % Util::utf8ToUnicode(raw)).str();
		}

		class wstringstream
		{
		public:
			wstringstream() {}
			template <typename T> wstringstream& operator<<(const T& t);
			std::wstring str() const { return w.str(); }
			void str (const std::wstring& s) { w.str(s); }

		private:

			std::wstringstream w;
		};

		template <typename T>
		inline wstringstream& 
		wstringstream::operator<<(const T& t)
		{
			w << t;
			return *this;
		}

		template <>
		inline wstringstream&
		wstringstream::operator<<(const std::string& str)
		{
			w << Util::utf8ToUnicode(str.c_str()); 
			return *this;
		}
	}
}