#include "boost/assign/list_of.hpp"
#include "MIMEResolver.h"

using namespace Util;

MIMEResolver::TypeMap MIMEResolver::types = boost::assign::map_list_of
    (L"html", "text/html")
    (L"htm", "text/html")
    (L"css", "text/css")
    (L"xml", "text/xml")
    (L"js", "text/javascript")
    (L"txt", "text/plain")
    // images
    (L"jpg", "image/jpeg")
    (L"gif", "image/gif")
    (L"ico", "image/x-icon")
    (L"png","image/png")
    // audio
    (L"mp3", "audio/mpeg")
    (L"mp2", "audio/mpeg")
    (L"mp1", "audio/mpeg")
    (L"wav", "audio/wav")
    (L"ogg", "application/ogg")
    (L"flac", "audio/x-flac")
    (L"m4a", "audio/mp4")
    (L"ape", "audio/monkeys-audio")
    (L"aac", "audio/x-aac")
    (L"wma", "audio/x-ms-wma")
    // playlist
    (L"m3u", "audio/m3u")
    (L"pls", "audio/scpls")
    //(L"b4s", true),
    // video
    (L"avi", "video/avi")
    (L"mpg", "video/mpg")
    (L"mp4", "video/mp4")
    (L"wmv", "video/x-ms-wvx")
	(L"mov", "video/quicktime");

bool MIMEResolver::valid(const std::wstring& path)
{
    if (MIMEResolver::types.find(MIMEResolver::parseExtension(path)) != MIMEResolver::types.end())
        return true;
    return false;
}

std::string MIMEResolver::MIMEType(const std::wstring& path)
{
    MIMEResolver::TypeMap::iterator iter;
    if (( iter = MIMEResolver::types.find(MIMEResolver::parseExtension(path))) != MIMEResolver::types.end())
        return iter->second;
    return "application/octet-stream";
}

std::wstring MIMEResolver::parseExtension(const std::wstring& filename) 
{
    std::wstring::size_type pos = 0;
    pos = filename.rfind(L'.');
    if (pos != std::wstring::npos && pos < filename.size())
        return filename.substr(pos+1);
    return L"";
}