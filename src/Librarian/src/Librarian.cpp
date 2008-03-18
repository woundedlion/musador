#include "boost/filesystem.hpp"
#include "Librarian.h"
#include "Network/Network.h"
#include "Indexer/ConsoleProgressReporter.h"
#include "Server/PipeListener.h"

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
}

Librarian::~Librarian()
{
	Config::destroy();
	Musador::Network::destroy();
	Logger::destroy();
}

int 
Librarian::run(unsigned long argc, wchar_t * argv[])
{
	this->enable();
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
//		this->gui.postMessage(WM_APP_SERVERUP);
	}
}

void Librarian::disable()
{
	if (NULL != this->server.get())
	{
		this->server->stop();
		server->waitForStop();
		this->server.reset();
//		this->gui.postMessage(WM_APP_SERVERDOWN);
	}

}