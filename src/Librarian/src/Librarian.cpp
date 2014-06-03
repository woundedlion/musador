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
db(db_path()),
cfg(loadConfig()),
controller(cfg),
server(cfg.server)
{
    Network::instance();
    IO::Proactor::instance()->start();
}

Librarian::~Librarian()
{
    IO::Proactor::instance()->stop();
    IO::Proactor::destroy();
	Network::destroy();
}

int 
Librarian::run(unsigned long argc, wchar_t * argv[])
{	
    server.start();

	boost::shared_ptr<IO::Listener> listener(new GUIListener());
    listener->beginAccept(boost::bind(&Librarian::onGUIAccept,this,_1,_2));
    waitForStop(); 

	server.stop();
    server.waitForStop();
    
	notifyGUI<GUIMsgDisabledNotify>();  
    return 0;
}

const std::wstring
Librarian::data_path()
{
	static auto r = (fs::wpath(Util::pathToDataDir()) / L"\\Musador").wstring();
	return r;
}

const std::wstring
Librarian::cfg_path()
{
	static auto r = (fs::wpath(data_path()) / L"librarian.cfg").wstring();
	return r;
}

const std::wstring
Librarian::db_path()
{
	static auto r = (fs::wpath(data_path()) / L"librarian.db").wstring();
	return r;
}

LibrarianConfig
Librarian::loadConfig()
{
	LibrarianConfig cfg;
	if (!fs::exists(cfg_path())) {
		configDefaults(cfg);
		saveConfig(cfg);
	}

	cfg.server.controller = &controller;

	try {
		std::ifstream in(cfg_path());
		storm::json::InputArchive json(in);
		json >> cfg;
		return cfg;
	}
	catch (const std::runtime_error& e) {
		LOG(Error) << "Failed to load config: " << e.what();
		return cfg;
	}
}


void
Librarian::saveConfig(const LibrarianConfig& cfg)
{
	try {
		std::ofstream out(cfg_path());
		storm::json::OutputArchive json(out);
		json << cfg;
	}
	catch (const std::runtime_error& e) {
		LOG(Error) << "Failed to save config: " << e.what();
	}
}

void
Librarian::configDefaults(LibrarianConfig& cfg)
{
	cfg.dataDir = data_path();

	HTTP::Config site;
	site.addr = "0.0.0.0";
	site.port = 5152;
	site.documentRoot = (fs::wpath(cfg.dataDir) / L"html").wstring();
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
	lib.dataFile = (fs::wpath(cfg.dataDir) / L"bighurt.db").wstring();
	std::vector<std::wstring> targets;
	targets.push_back(L"C:\\music\\~tagged");
	lib.targets = targets;
	LibrarianConfig::LibraryCollection libraries;
	libraries.push_back(lib);
	cfg.libraries = libraries;
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
        msgAccept->listener->beginAccept(boost::bind(&Librarian::onGUIAccept,this,_1,_2));
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