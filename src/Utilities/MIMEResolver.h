#ifndef MIMERESOLVER_229871CB_8AA7_4989_917F_3D7C09D02FD9
#define MIMERESOLVER_229871CB_8AA7_4989_917F_3D7C09D02FD9

#include <string>
#include <map>

namespace Util
{
    class MIMEResolver
    {
    public:

        /// @brief The mapping of file extensions to MIME types
        typedef std::map<std::wstring,std::wstring> TypeMap;

        /// @brief Check if a path has a resolvable MIME type
        /// @param[in] path A URL or filesystem path
        /// @returns true if there is a known MIME type for the object indicated by path, false otherwise
        static bool valid(const std::wstring& path);

        /// @brief Get a MIME type from a path
        /// @param[in] path A URL or filesystem path
        /// @returns The MIME type for the object indicated by path
        static std::wstring MIMEType(const std::wstring& path);

    private:

        MIMEResolver();

        static TypeMap types;
        static std::wstring parseExtension(const std::wstring& filename);
    };
}

#endif