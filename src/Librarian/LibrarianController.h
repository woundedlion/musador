#ifndef LIBRARIAN_CONTROLLER_H_2B3977A0_7D74_4785_B61B_EECC28F9D290
#define LIBRARIAN_CONTROLLER_H_2B3977A0_7D74_4785_B61B_EECC28F9D290

#include <sstream>
#include <boost/serialization/nvp.hpp>
#include "Server/Controller.h"

namespace Musador
{

	class LibrarianController : public Controller
	{
	public:

		LibrarianController();

        /// @class A serializable error instance.
        class Error
        {
        public:

            Error& operator<<(const std::string& s);

            Error& operator<<(const std::wstring& s);

            /// Serialize this Error.
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