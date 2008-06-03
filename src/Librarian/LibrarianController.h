#ifndef LIBRARIAN_CONTROLLER_H_2B3977A0_7D74_4785_B61B_EECC28F9D290
#define LIBRARIAN_CONTROLLER_H_2B3977A0_7D74_4785_B61B_EECC28F9D290

#include <sstream>
#include <boost/serialization/nvp.hpp>
#include "Server/Controller.h"

namespace Musador
{
    /// @class LibrarianController
    /// @brief Implements the Controller interface for the Librarian application server.
    class LibrarianController : public Controller
    {
    public:

        /// @brief Constructor.
        LibrarianController();

        /// @class Error 
        /// @brief A serializable error instance for LibrarianController errors.
        class Error
        {
        public:

            /// @brief append a string to the error message.
            Error& operator<<(const std::string& s);

            /// @brief append a wide-character string to the error message.
            /// @note The wide-character string will be UTF-8 encoded upon serialization to XML
            Error& operator<<(const std::wstring& s);

            /// @brief Serialize this Error.
            /// @param[in] ar Destination archive for the serialized data.
            /// @param[in] version The version of the archive. Used by boost::serailization version tracking.
            template<class Archive>
            void serialize(Archive & ar, const unsigned int version)
            {
                ar & boost::serialization::make_nvp("error_string", errStr.str());
            }

        private:

            std::stringstream errStr;
        };

    private:

        bool info(HTTP::Env& env);
        bool config(HTTP::Env& env);
        bool index(HTTP::Env& env);
        bool getLibraryXML(HTTP::Env& env);
        bool getLibraryStatsXML(HTTP::Env& env);
    };
}

#endif