#include <boost/filesystem.hpp>
#include <boost/bind.hpp>

#include "Librarian.h"
#include "Network/Network.h"
#include "Indexer/ConsoleProgressReporter.h"

#include "Protocol/GUIMessages.h"

#include "Logger/Logger.h"
#define LOG_SENDER "Librarian"
using namespace Musador;

namespace fs = boost::filesystem;

Librarian::Librarian() :
WindowsService(L"Musador Librarian")
{
	Network::instance();
	MIMEResolver::instance();

	// load config or generate defaults
    fs::wpath dataPath(Util::pathToDataDir() + L"\\Musador");
    if (!fs::exists(dataPath))
    {
        fs::create_directories(dataPath);
    }
    std::wstring cfgPath = (dataPath / L"Librarian.xml").file_string();
    Config * cfg = Config::instance();
    if (!fs::exists(cfgPath) || !cfg->load(cfgPath))
	{
        cfg->librarian.dataDir = dataPath.directory_string();
		this->configDefaults(*cfg);
		if (!cfg->save(cfgPath))
		{
			LOG(Error) << "Could not save configuration to " << cfgPath;
		}
	}

    this->controller.reset(new LibrarianController());
	cfg->server.controller = this->controller.get();

	this->server.reset(new Server(cfg->server));

	// Start 2 worker threads
	Proactor::instance()->start(2);
}

Librarian::~Librarian()
{
	Proactor::instance()->stop();
	Proactor::destroy();

	Config::destroy();
	Musador::Network::destroy();
	MIMEResolver::destroy();
}

int 
Librarian::run(unsigned long argc, wchar_t * argv[])
{	
	this->server->start();

	// Start listening for incoming gui connections
	this->listener.reset(new GUIListener());
	this->listener->beginAccept(boost::bind(&Librarian::onGUIAccept,this,_1,_2));

	// Wait until SCM or ctrl-c shuts us down
	this->waitForStop(); 

	this->server->stop();
	this->server->waitForStop();
	this->notifyGUI<GUIMsgDisabledNotify>();  

	return 0;
}

void 
Librarian::configDefaults(Config& cfg)
{
	HTTPConfig site;
	site.addr = "0.0.0.0";
	site.port = 5152;
    site.documentRoot = (fs::wpath(cfg.librarian.dataDir.get()) / L"html").directory_string();
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
    lib.dataFile = (fs::wpath(cfg.librarian.dataDir.get()) / L"bighurt.db").file_string();
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

	for (std::vector<std::wstring>::const_iterator iter = paths.begin(); iter != paths.end(); ++iter)
	{
		indexer.addRootTarget(*iter);
	}
	
	indexer.reindex();

	ConsoleProgressReporter reporter(indexer);
	reporter.run();
	
	indexer.waitDone();

	IndexerProgress p = indexer.progress();
    unsigned int duration = (std::clock() - p.startTime) / CLOCKS_PER_SEC;
    LOG(Info) << "\n" "Indexing of " << p.numFiles << " files in " << p.numDirs << " directories (" << Util::bytesToString(p.bytes) << ") completed in " << duration << " seconds";
}

void
Librarian::onGUIAccept(boost::shared_ptr<IOMsg> msg, boost::any /*tag = NULL*/)
{
    switch (msg->getType())
    {
	case IO_PIPE_ACCEPT_COMPLETE:
		{
			boost::shared_ptr<IOMsgPipeAcceptComplete>& msgAccept = boost::shared_static_cast<IOMsgPipeAcceptComplete>(msg);

			this->gui = boost::shared_static_cast<GUIConnection>(msgAccept->conn);
			this->gui->setHandler(boost::bind(&Librarian::onGUIMsg,this,_1));

            this->notifyGUI<GUIMsgEnabledNotify>();        

			// Keep listening for new connections
			this->listener->beginAccept(boost::bind(&Librarian::onGUIAccept,this,_1,_2));
			
			this->gui->beginRead();

		}
		break;
    case IO_ERROR:
        {
            boost::shared_ptr<IOMsgError> msgErr(boost::shared_static_cast<IOMsgError>(msg));
            LOG(Error) << "Error accepting GUI Connection: " << msgErr->err;
        }
        break;
    }
}

void
Librarian::onGUIMsg(boost::shared_ptr<GUIMsg> msg)
{
    switch (msg->getType())
    {
	case GUI_MSG_DISABLE_REQ:
		this->stop();
		break;
    }
}