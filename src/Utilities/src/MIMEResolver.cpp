#include "boost/assign/list_of.hpp"
#include "MIMEResolver.h"

using namespace Util;

MIMEResolver::TypeMap MIMEResolver::types = boost::assign::map_list_of
    (L"html", L"text/html")
    (L"htm", L"text/html")
    (L"css", L"text/css")
    (L"xml", L"text/xml")
    (L"js", L"text/javascript")
    (L"txt", L"text/plain")
    // images
    (L"jpg", L"image/jpeg")
    (L"gif", L"image/gif")
    (L"ico", L"image/x-icon")
    (L"png", L"image/png")
    // audio
    (L"mp3", L"audio/mpeg")
    (L"mp2", L"audio/mpeg")
    (L"mp1", L"audio/mpeg")
    (L"wav", L"audio/wav")
    (L"ogg", L"application/ogg")
    (L"flac", L"audio/x-flac")
    (L"m4a", L"audio/mp4")
    (L"ape", L"audio/monkeys-audio")
    (L"aac", L"audio/x-aac")
    (L"wma", L"audio/x-ms-wma")
    // playlist
    (L"m3u", L"audio/m3u")
    (L"pls", L"audio/scpls")
    //(L"b4s", true),
    // video
    (L"avi", L"video/avi")
    (L"mpg", L"video/mpg")
    (L"mp4", L"video/mp4")
    (L"wmv", L"video/x-ms-wvx")
	(L"mov", L"video/quicktime");

bool MIMEResolver::valid(const std::wstring& path)
{
    if (MIMEResolver::types.find(MIMEResolver::parseExtension(path)) != MIMEResolver::types.end())
        return true;
    return false;
}

std::wstring MIMEResolver::MIMEType(const std::wstring& path)
{
    MIMEResolver::TypeMap::iterator iter;
    if (( iter = MIMEResolver::types.find(MIMEResolver::parseExtension(path))) != MIMEResolver::types.end())
        return iter->second;
    return L"application/octet-stream";
}

std::wstring MIMEResolver::parseExtension(const std::wstring& filename) 
{
    std::wstring::size_type pos = 0;
    pos = filename.rfind(L'.');
    if (pos != std::wstring::npos && pos < filename.size())
        return filename.substr(pos+1);
    return L"";
}