#ifndef LIBRARIAN_F34BA23D_EBB0_4871_89C1_20AB9FDC155D
#define LIBRARIAN_F34BA23D_EBB0_4871_89C1_20AB9FDC155D

#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/shared_ptr.hpp>

#include "Utilities/WindowsService.h"
#include "Indexer/Indexer.h"
#include "Server/Server.h"
#include "Utilities/Console.h"
#include "Config/Config.h"
#include "LibrarianController.h"
#include "Protocol/GUIListener.h"
#include "Protocol/GUIConnection.h"

using namespace Musador;
namespace Musador
{
    class PipeListener;

    /// @class Librarian
    /// @brief The daemon component of the Musador system.
    /// This module contains the web server, services requests from web clients, manages config data,
    /// and manages library index files.
    class Librarian : public WindowsService<Librarian>
    {

        friend class WindowsService<Librarian>;

    public:

        /// @brief Constructor.
        Librarian();

        /// @brief Destructor.
        ~Librarian();

        /// @brief Run the main thread of the Librarian.
        /// @note This function will not return until the Librarian is shut down.
        int run(unsigned long argc, wchar_t * argv[]);

        /// @brief Run an indexing job on the specified paths, writing index data to the specified output file.
        /// @note This function behaves synchronously, returning when the indexing job is complete.
        void index(const std::wstring& outfile,const std::vector<std::wstring>& paths);

        /// @brief Populate the provided Config class with default values
        void configDefaults(Config& cfg);

    private:

        void onGUIAccept(boost::shared_ptr<IOMsg> msg, boost::any tag);
        void onGUIMsg(boost::shared_ptr<GUIMsg> msg);

        template <typename T>
        void notifyGUI();

        boost::shared_ptr<GUIListener> listener;
        boost::shared_ptr<GUIConnection> gui;
        boost::shared_ptr<Server> server;
        boost::shared_ptr<LibrarianController> controller;

    };

    /// @brief Send a message to the GUI process.
    /// @param The type of message to send to the GUI process.
    template <typename T>
    void Librarian::notifyGUI()
    {
        if (NULL != this->gui)
        {
            boost::shared_ptr<T> msg(new T());
            std::stringstream msgData;
            boost::archive::binary_oarchive ar(msgData);
            ar & msg;
            this->gui->beginWrite(msgData);
        }
    }

}

#endif