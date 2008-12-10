#ifndef BASE64_H_BD0CAAF7_87FA_46e9_9917_0AC6E4B3734D
#define BASE64_H_BD0CAAF7_87FA_46e9_9917_0AC6E4B3734D

#include <string>

namespace Util
{
    std::string base64Encode(std::string const& s);
    std::string base64Decode(std::string const& s);
}

#endif