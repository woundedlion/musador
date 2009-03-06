#ifndef BASE64_H_BD0CAAF7_87FA_46e9_9917_0AC6E4B3734D
#define BASE64_H_BD0CAAF7_87FA_46e9_9917_0AC6E4B3734D

#include <string>

namespace Util
{
    /// @brief Base64-encode a string
    /// @param[in] s A string to Base64-encode
    /// @returns A Base64-encoded copy of s
    std::string base64Encode(std::string const& s);
    
    /// @brief Decode a Base64-encoded string
    /// @param[in] s A Base64-encoded string to decode
    /// @returns A decoded copy of s
    std::string base64Decode(std::string const& s);
}

#endif