#include <locale>
#include <sstream>
#include <cstdlib>

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <objbase.h>
#endif

#include <boost/thread/tss.hpp>
#define BOOST_UTF8_BEGIN_NAMESPACE namespace boost {
#define BOOST_UTF8_DECL
#define BOOST_UTF8_END_NAMESPACE }
#include <boost/detail/utf8_codecvt_facet.hpp>
#include <boost/detail/utf8_codecvt_facet.ipp>
#undef BOOST_UTF8_END_NAMESPACE
#undef BOOST_UTF8_DECL
#undef BOOST_UTF8_BEGIN_NAMESPACE

#include "Utilities/Util.h"

typedef boost::utf8_codecvt_facet codecvt_t;

namespace Util
{
    boost::thread_specific_ptr<std::locale> loc(&Util::cleanupUtf8Locale);
}

void Util::cleanupUtf8Locale(std::locale * /* loc */)
{
    delete Util::loc.get();	
}

std::string Util::unicodeToUtf8(const std::wstring& in)
{
    return unicodeToUtf8(in.c_str());
}

std::string Util::unicodeToUtf8(const wchar_t * in)
{
    if (NULL == Util::loc.get()){
        Util::loc.reset(new std::locale(std::locale(),new codecvt_t));
    }

    std::mbstate_t state;
    const wchar_t * i_next = in;
    char * out = new char[(::wcslen(in) * 4) + 1];
    char * o_next = out;
    std::use_facet<codecvt_t>(*Util::loc).out(state, in, &in[::wcslen(in)], i_next, out, &out[(::wcslen(in) * 4) + 1], o_next);
    std::string result(out,static_cast<size_t>(o_next - out));
    delete [] out;
    return result;
}

std::string Util::unicodeToUtf8(wchar_t in)
{
    wchar_t str[2];
    str[0] = in;
    str[1] = L'\0';
    return Util::unicodeToUtf8(str);
}

std::wstring Util::utf8ToUnicode(const std::string& in)
{
    return utf8ToUnicode(in.c_str());
}

std::wstring Util::utf8ToUnicode(const char * in)
{
    if (NULL == Util::loc.get()){
        Util::loc.reset(new std::locale(std::locale(),new codecvt_t));
    }
    std::mbstate_t state;
    const char * i_next;
    wchar_t * out = new wchar_t[::strlen(in) + 1];
    wchar_t * o_next;
    std::use_facet<codecvt_t>(*Util::loc).in(state, in, &in[::strlen(in)], i_next, out, &out[::strlen(in) + 1], o_next);
    std::wstring result(out,static_cast<size_t>(o_next - out));
    delete [] out;
    return result;
}

std::wstring Util::utf8ToUnicode(char in)
{
    char str[2];
    str[0] = in;
    str[1] = L'\0';
    return Util::utf8ToUnicode(str);
}

std::wstring Util::escapeQuotes(const std::wstring& in)
{
    std::wstring::size_type pos = 0;
    if ((pos = in.find(L'\'')) != std::wstring::npos)
    {
        std::wstring str;
        str.reserve(2 * in.size());
        str.assign(in,0,pos);
        for (std::wstring::const_iterator iter = in.begin() + pos; iter != in.end(); iter++)
        {
            wchar_t c;
            if ((c = *iter) == L'\'')
            {
                str += L"''";
            } else
                str += c;
        }
        return str;
    }
    return in;
}

std::wstring Util::escapeQuotes(const wchar_t * inStr)
{
    std::wstring in(inStr);
    return Util::escapeQuotes(in);
}

std::wstring Util::bytesToString(unsigned long long bytes)
{	
    std::wstringstream r;
    int p = 0;

    long double v;
    if (bytes >= 1024)
    {
        // to avoid overflow from unsigned long long to unisgned long double we divide once before assignment
        v = bytes / (float)1024;
        ++p;
        while (1024 <= v)
        {
            v /= (float)1024;
            ++p;
        }
    } else
        v = static_cast<unsigned long double>(bytes);

    r << ::floor(v * 100) / 100;
    switch (p)
    {
    case 0:
        r << " bytes";
        break;
    case 1:
        r << " KiB";
        break;
    case 2:
        r << " MiB";
        break;
    case 3:
        r << " GiB";
        break;
    case 4:
        r << " TiB";
        break;
    case 5:
        r << " PiB";
        break;
    case 6:
        r << " EiB";
        break;
    case 7:
        r << " ZiB";
        break;
    case 8:
        r << " YiB";
        break;
    }

    return r.str();
}

void Util::tokenize(const std::string& str,
                    std::vector<std::string>& tokens,
                    const std::string& delimiters = " ")
{
    // Skip delimiters at beginning.
    std::string::size_type lastPos = str.find_first_not_of(delimiters, 0);
    // Find first "non-delimiter".
    std::string::size_type pos = str.find_first_of(delimiters, lastPos);

    while (std::string::npos != pos || std::string::npos != lastPos)
    {
        // Found a token, add it to the vector.
        tokens.push_back(str.substr(lastPos, pos - lastPos));
        // Skip delimiters.  Note the "not_of"
        lastPos = str.find_first_not_of(delimiters, pos);
        // Find next "non-delimiter"
        pos = str.find_first_of(delimiters, lastPos);
    }
}

int Util::hexToDec(char digit) {
    int n;
    switch (digit) {
                        case '0':
                            n = 0;
                            break;
                        case '1':
                            n = 1;
                            break;
                        case '2':
                            n = 2;
                            break;
                        case '3':
                            n = 3;
                            break;
                        case '4':
                            n = 4;
                            break;
                        case '5':
                            n = 5;
                            break;
                        case '6':
                            n = 6;
                            break;
                        case '7':
                            n = 7;
                            break;
                        case '8':
                            n = 8;
                            break;
                        case '9':
                            n = 9;
                            break;
                        case 'A':
                        case 'a':
                            n = 10;
                            break;
                        case 'B':
                        case 'b':
                            n = 11;
                            break;
                        case 'C':
                        case 'c':
                            n = 12;
                            break;
                        case 'D':
                        case 'd':
                            n = 13;
                            break;
                        case 'E':
                        case 'e':
                            n = 14;
                            break;
                        case 'F':
                        case 'f':
                            n = 15;
                            break;
                        default:
                            n = -1;
    }
    return n;
}

char Util::hexToChar(const std::string& hex) {
    std::string::const_iterator iter;
    if (hex.empty()) return 0;
    iter = hex.begin();
    int sum = hexToDec(*iter);
    for (; iter < hex.end()-1; iter++) {
        sum = (sum*16)+hexToDec(*(iter+1)); // Horner's Algorithm
    }
    return (char)sum;
}

bool Util::parseNameValuePair(const std::string& pairString, char delim, std::pair<std::string,std::string>& resultPair) {
    size_t delimPos;
    if ((delimPos = pairString.find(delim)) != std::string::npos) {
        std::string name = pairString.substr(0,delimPos);
        delimPos++;
        while ((delimPos < pairString.length()) && (pairString[delimPos] == ' '))
            delimPos++;
        std::string value = pairString.substr(delimPos);
        resultPair = std::make_pair(name,value);
        return true;
    } else return false;
}

void Util::replaceChar(std::string& str, char from, char to) {
    for (std::string::iterator iter = str.begin(); iter < str.end(); iter++ ) {
        if (*iter == from)
            *iter = to;
    }
}

void Util::genGUID(GUID& uuid) {
    ::CoCreateGuid(&uuid);
}

void Util::genGUID(std::string& guidStr) {
    GUID guid;
    HRESULT err;
    err = ::CoCreateGuid(&guid);
    unsigned char * str;
    err = ::UuidToStringA((UUID *)&guid,&str);
    if (err == RPC_S_OK) {
        guidStr = (char *)str;
        ::RpcStringFreeA(&str);
    } 
}

void Util::guidToString(const GUID& guid, std::string& guidStr) {
    unsigned char * str;
    int err = ::UuidToStringA((UUID *)&guid,&str);
    if (err == RPC_S_OK) {
        guidStr = (char *)str;
        ::RpcStringFreeA(&str);
    } 
}

INT16 Util::ReadInt16(BYTE * buf) {
    INT16 val = 0;	
    ::memcpy((void *)&val,buf,2);
    val = ::ntohs(val);
    return val;
}

void Util::WriteInt16(BYTE * buf, INT16 val) {
    val = ::htons(val);
    ::memcpy(buf,(const void *)&val,2);
}

INT32 Util::ReadInt32(BYTE * buf) {
    INT32 val = 0;	
    ::memcpy((void *)&val,buf,4);
    val = ::ntohl((u_long)val);
    return val;
}

void Util::WriteInt32(BYTE * buf, INT32 val) {
    val = ::htonl((u_long)val);
    ::memcpy(buf,(const void *)&val,4);
}

std::wstring
Util::pathToDataDir()
{
    std::wstring path;
    wchar_t buf[MAX_PATH];
    BOOL r = ::SHGetFolderPathW(NULL, CSIDL_COMMON_APPDATA, NULL, SHGFP_TYPE_CURRENT, buf);
    if (S_OK == r)
    {
        path = buf;
    }
    return path;
}
