#include "MIMEResolver.h"

MIMEResolver::TypeMap MIMEResolver::types;

MIMEResolver::MIMEResolver()
{
	// text
	MIMEResolver::types[L"html"] = L"text/html";
	MIMEResolver::types[L"htm"] = L"text/html";
	MIMEResolver::types[L"css"] = L"text/css";
	MIMEResolver::types[L"xml"] = L"text/xml";
	MIMEResolver::types[L"js"] = L"text/javascript";
	MIMEResolver::types[L"txt"] = L"text/plain";
	// images
	MIMEResolver::types[L"jpg"] = L"image/jpeg";
	MIMEResolver::types[L"gif"] = L"image/gif";
	MIMEResolver::types[L"ico"] = L"image/x-icon";
	MIMEResolver::types[L"png"] = L"image/png";
	// audio
	MIMEResolver::types[L"mp3"] = L"audio/mpeg";
	MIMEResolver::types[L"mp2"] = L"audio/mpeg";
	MIMEResolver::types[L"mp1"] = L"audio/mpeg";
	MIMEResolver::types[L"wav"] = L"audio/wav";
	MIMEResolver::types[L"ogg"] = L"application/ogg";
	MIMEResolver::types[L"flac"] = L"audio/x-flac";
	MIMEResolver::types[L"m4a"] = L"audio/mp4";
	MIMEResolver::types[L"ape"] = L"audio/monkeys-audio";
	MIMEResolver::types[L"aac"] = L"audio/x-aac";
	MIMEResolver::types[L"wma"] = L"audio/x-ms-wma";
	// playlist
	MIMEResolver::types[L"m3u"] = L"audio/m3u";
	MIMEResolver::types[L"pls"] = L"audio/scpls";
	//MIMEResolver::types[L"b4s"] = true;
	// video
	MIMEResolver::types[L"avi"] = L"video/avi";
	MIMEResolver::types[L"mpg"] = L"video/mpg";
	MIMEResolver::types[L"mp4"] = L"video/mp4";
	MIMEResolver::types[L"wmv"] = L"video/x-ms-wvx";
	MIMEResolver::types[L"mov"] = L"video/quicktime";
}

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