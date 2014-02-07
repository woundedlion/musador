#include <boost/filesystem.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>

#include "Librarian.h"
#include "Network/Network.h"
#include "Indexer/ConsoleProgressReporter.h"

#include "Protocol/GUIMessages.h"

#include "Logger/Logger.h"
#define LOG_SENDER "Librarian"
using namespace Musador;

namespace fs = boost::filesystem;

Librarian::Librarian() :
UI::Daemon<Librarian>(L"Musador Librarian"),
db(cfg_path())
{
    Network::instance();

	load_config();
    
	controller.reset(new LibrarianController());
	server.reset(new Server(cfg.server, controller));

    IO::Proactor::instance()->start();
}

Librarian::~Librarian()
{
    IO::Proactor::instance()->stop();
    IO::Proactor::destroy();

    Config::destroy();
    Network::destroy();
}

int 
Librarian::run(unsigned long argc, wchar_t * argv[])
{	
    server->start();

    // Start listening for incoming gui connections
    listener.reset(new GUIListener());
    listener->beginAccept(boost::bind(&Librarian::onGUIAccept,this,_1,_2));

    // Wait until SCM or ctrl-c shuts us down
    waitForStop(); 

    server->stop();
    server->waitForStop();
    notifyGUI<GUIMsgDisabledNotify>();  

    return 0;
}

std::wstring
Librarian::cfg_path()
{
	fs::wpath data_path(Util::pathToDataDir() + L"\\Musador";
	if (!fs::exists(data_path)) {
		fs::create_directories(data_path);
	}
	return (data_path / L"librarian.db").wstring();
}

void 
Librarian::load_config()
{
    HTTPConfig site;
    site.addr = "0.0.0.0";
    site.port = 5152;
    site.documentRoot = (fs::wpath(cfg.librarian.dataDir.get()) / L"html").wstring();
    site.requireAuth = false;
    site.realm = L"Musador";

    HTTP::User u("admin");
    u.setPassword("password");
    HTTP::UserCollection users;
    users[u.getUsername()] = u;
    site.users = users;

    ServerConfig::HTTPSiteCollection sites;
    sites.push_back(site);
    cfg.server.sites = sites;

    LibraryConfig lib;
    lib.id = 0;
    lib.nickname = L"bighurt";
    lib.dataFile = (fs::wpath(cfg.librarian.dataDir.get()) / L"bighurt.db").wstring();
    std::vector<std::wstring> targets;
    targets.push_back(L"C:\\music\\~tagged");
    lib.targets = targets;
    LibrarianConfig::LibraryCollection libraries;
    libraries[lib.id] = lib;
    cfg.librarian.libraries = libraries;
}

void 
Librarian::index(const std::wstring& outfile,const std::vector<std::wstring>& paths)
{
    Indexer indexer(outfile);

    for (auto path : paths) {
		indexer.addTarget(path);
    }

    indexer.reindex();

    ConsoleProgressReporter reporter(indexer);
    reporter.run();

    IndexerProgress p = indexer.getProgress();
    unsigned int duration = (std::clock() - p.startTime) / CLOCKS_PER_SEC;
    LOG(Info) << "\n" "Indexing of " << p.numFiles << " files in " << p.numDirs << " directories (" << Util::bytesToString(p.bytes) << ") completed in " << duration << " seconds";
}

void
Librarian::onGUIAccept(boost::shared_ptr<IO::Msg> msg, boost::any /*tag = NULL*/)
{
    assert(msg->getType() == IO::MSG_PIPE_ACCEPT_COMPLETE);
    boost::shared_ptr<IO::MsgPipeAcceptComplete>& msgAccept = boost::static_pointer_cast<IO::MsgPipeAcceptComplete>(msg);
    if (msgAccept->isError()) {
        LOG(Error) << "Error accepting GUI Connection: " << msgAccept->getError();
    }
    else 
    {
        // Notify the GUI of the new connection
        gui = boost::static_pointer_cast<GUIConnection>(msgAccept->conn);
        gui->setHandler(boost::bind(&Librarian::onGUIMsg,this,_1));
        notifyGUI<GUIMsgEnabledNotify>();        

        // Keep listening
        listener->beginAccept(boost::bind(&Librarian::onGUIAccept,this,_1,_2));
        gui->beginRead();
    }
}

void
Librarian::onGUIMsg(boost::shared_ptr<GUIMsg> msg)
{
    switch (msg->getType())
    {
    case GUI_MSG_DISABLE_REQ:
        stop();
        break;
    }
}