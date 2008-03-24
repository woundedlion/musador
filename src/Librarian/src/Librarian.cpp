#include <boost/filesystem.hpp>
#include <boost/bind.hpp>

#include "Librarian.h"
#include "Network/Network.h"
#include "Indexer/ConsoleProgressReporter.h"

#include "Server/ConnectionProcessor.h"
#include "Protocol/GUIListener.h"
#include "Protocol/GUIMessages.h"

#include "Logger/Logger.h"
#define LOG_SENDER "Librarian"
using namespace Musador;
namespace fs = boost::filesystem;

Librarian::Librarian() :
WindowsService(L"Musador Librarian")
{
#if _DEBUG
	Logger::instance()->setLevel(Debug);
#endif

	Network::instance();

	// load config or generate defaults
	std::wstring cfgPath = L"Librarian.xml";
	Config * cfg = Config::instance();
	if (!fs::exists(cfgPath) || !cfg->load(cfgPath))
	{
		this->configDefaults(*cfg);
		if (!cfg->save(cfgPath))
		{
			LOG(Error) << "Could not save configuration to " << cfgPath;
		}
	}

	cfg->server.controller = &this->controller;

	// Start 2 worker threads
	Proactor::instance()->start(2);
}

Librarian::~Librarian()
{
	Proactor::instance()->stop();
	Proactor::destroy();

	Config::destroy();
	Musador::Network::destroy();
	Logger::destroy();
}

int 
Librarian::run(unsigned long argc, wchar_t * argv[])
{	
	this->enable();

	boost::shared_ptr<GUIListener> GUIListener(new GUIListener());
	GUIListener->beginAccept(boost::bind(&Librarian::onAcceptGUIConnection,this,_1,_2));

	this->waitForStop(); // Wait until SCM or ctrl-c shuts us down
	this->disable();

	return 0;
}

void 
Librarian::configDefaults(Config& cfg)
{
	cfg.server.sites.clear();
	HTTPConfig site;
	site.addr = "0.0.0.0";
	site.port = 5152;
	site.documentRoot = L"html";
	site.requireAuth = true;
	site.realm = L"Musador";
	cfg.server.sites.push_back(site);
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
	LOG(Info) << "Indexing of " << p.numFiles << " files in " << p.numDirs << " directories (" << Util::bytesToString(p.bytes) << ") completed in " << p.duration << " seconds";
}

void
Librarian::enable()
{
	if (NULL == this->server.get())
	{
		this->server.reset(new Server(Config::instance()->server));
		this->server->start();
	}

        this->notifyGUI<GUIMsgEnabledNotify>();        
}

void Librarian::disable()
{
	if (NULL != this->server.get())
	{
		this->server->stop();
		server->waitForStop();
		this->server.reset();
	}
    
	this->notifyGUI<GUIMsgDisabledNotify>();        
}

void
Librarian::onAcceptGUIConnection(boost::shared_ptr<IOMsg> msg, boost::any tag)
{
    switch (msg->getType())
    {
    case IO_PIPE_ACCEPT_COMPLETE:
        {
            boost::shared_ptr<IOMsgPipeAcceptComplete>& msgAccept = boost::shared_static_cast<IOMsgPipeAcceptComplete>(msg);

            this->gui = boost::shared_static_cast<GUIConnection>(msgAccept->conn);
			this->gui->setHandler(boost::bind(&Librarian::onGUIMsg,this,_1));

            if (NULL != this->server.get())
            {
                this->notifyGUI<GUIMsgEnabledNotify>();        
            }
            else
            {
                this->notifyGUI<GUIMsgDisabledNotify>();        
            }

            // Keep listening for new connections
            msgAccept->listener->beginAccept(boost::bind(&Librarian::onAcceptGUIConnection,this,_1,_2));

			// Start the connection state machine
			msgAccept->conn->accepted();
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
	case GUI_MSG_ENABLE_REQ:
		this->enable();
		break;

	case GUI_MSG_DISABLE_REQ:
		this->disable();
		break;
    }
}