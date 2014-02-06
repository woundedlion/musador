#ifndef LIBRARIAN_CONTROLLER_H_2B3977A0_7D74_4785_B61B_EECC28F9D290
#define LIBRARIAN_CONTROLLER_H_2B3977A0_7D74_4785_B61B_EECC28F9D290

#include <sstream>
#include <boost/shared_ptr.hpp>
#include <boost/serialization/nvp.hpp>
#include "Server/Controller.h"
#include "Indexer/Indexer.h"

namespace Musador
{
    /// @class LibrarianController
    /// @brief Implements the Controller interface for the Librarian application server.
    class LibrarianController : public Controller
    {
    public:

        /// @brief Constructor.
        LibrarianController();

    private:

		void loadConfig();
		void bindHandlers();

		bool dumpRequest(HTTP::Env& env);

        bool reindex(HTTP::Env& env);
        bool cancelIndex(HTTP::Env& env);

        bool getConfig(HTTP::Env& env);
        bool getIndexProgressXML(HTTP::Env& env);
        bool getLibraryXML(HTTP::Env& env);
        bool getLibraryStatsXML(HTTP::Env& env);

        boost::shared_ptr<Indexer> indexer;

    };
}

#endif