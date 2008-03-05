#ifndef UTIL_FE6E0F31_B4FB_46d9_9B96_610F1B1EB37D
#define UTIL_FE6E0F31_B4FB_46d9_9B96_610F1B1EB37D

#include <string>
#include <vector>
#ifdef WIN32
#include "winsock2.h"
	#include <rpc.h>
	#pragma comment(lib,"Rpcrt4.lib")
	#pragma comment(lib,"ws2_32.lib")
#endif

namespace Util
{
	void cleanupUtf8Locale(std::locale *);

	std::string unicodeToUtf8(const std::wstring& in);

	std::string unicodeToUtf8(const wchar_t * in);

	std::string unicodeToUtf8(wchar_t in);

	std::wstring utf8ToUnicode(const std::string& in);

	std::wstring utf8ToUnicode(const char * in);

	std::wstring utf8ToUnicode(char in);

	std::wstring escapeQuotes(const std::wstring& in);

	std::wstring escapeQuotes(const wchar_t * in);

	std::wstring bytesToString(unsigned long long bytes);

	void tokenize(const std::string& str,std::vector<std::string>& tokens,const std::string& delimiters);

	int hexToDec(char digit);

	char hexToChar(const std::string& hex);

	void base64Encode(std::string& raw);

	bool parseNameValuePair(const std::string& pairString, char delim, std::pair<std::string,std::string>& resultPair);

	void replaceChar(std::string& str, char from, char to);

	void genGUID(GUID& uuid);

	void genGUID(std::string& guidStr);

	void guidToString(const GUID& guid, std::string& guidStr);

	INT16 ReadInt16(BYTE * buf);

	void WriteInt16(BYTE * buf, INT16 val);

	INT32 ReadInt32(BYTE * buf);

	void WriteInt32(BYTE * buf, INT32 val);
}

#endif