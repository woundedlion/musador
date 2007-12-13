#ifndef MIMERESOLVER_229871CB_8AA7_4989_917F_3D7C09D02FD9
#define MIMERESOLVER_229871CB_8AA7_4989_917F_3D7C09D02FD9

#include <map>
#include <string>

class MIMEResolver
{
public:

	typedef std::map<std::wstring,std::wstring> TypeMap;

	static void init();
	
	static bool valid(const std::wstring& path);

	static std::wstring MIMEType(const std::wstring& path);

private:

	MIMEResolver() {};
	~MIMEResolver() {};

	static bool inited;
	static TypeMap types;
	static std::wstring parseExtension(const std::wstring& filename);

};

#endif