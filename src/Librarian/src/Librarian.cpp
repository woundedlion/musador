#include "boost/filesystem.hpp"
#include "Librarian.h"
#include "Network/Network.h"
#include "Logger/Logger.h"
#define LOG_SENDER "Librarian"
#include "Indexer/ConsoleProgressReporter.h"

using namespace Musador;
namespace fs = boost::filesystem;

Librarian::Librarian() :
WindowsService(L"Musador Librarian")
{
#if _DEBUG
	Logger::instance()->setLevel(Debug);
#endif

	// load config or generate defaults
	std::wstring cfgPath = L"Librarian.xml";
	Config * cfg = Config::instance();
	if (fs::exists(cfgPath))
	{
		cfg->load(cfgPath);
	}
	else
	{
		this->configDefaults(*cfg);
		cfg->save(cfgPath);
	}
}

Librarian::~Librarian()
{
	Logger::destroy();
	Config::destroy();
}

int Librarian::run(unsigned long argc, wchar_t * argv[])
{

	Network::instance();

	{
		ServerConfig& cfg = Config::instance()->server;
		cfg.controller = &this->controller;
		std::auto_ptr<Server> server(new Server(cfg));
		server->start();
		this->waitForStop();

		server->stop();
		server->waitForStop();
	}

	Musador::Network::destroy();

	return 0;
}

void Librarian::configDefaults(Config& cfg)
{
	cfg.server.sites.clear();
	SiteConfig site;
	site.addr = "0.0.0.0";
	site.port = 5152;
	site.documentRoot = L"html";
	site.requireAuth = false;
	cfg.server.sites.push_back(site);
}

void Librarian::index(const std::wstring& outfile,const std::vector<std::wstring>& paths)
{
	Indexer indexer(outfile);

	for (std::vector<std::wstring>::const_iterator iter = paths.begin(); iter != paths.end(); ++iter)
	{
		indexer.addRootTarget(*iter);
	}
	
	indexer.reindex();

//	Console console;
//	console.setSignalHandler(&sigHandler);
	ConsoleProgressReporter reporter(indexer);
	reporter.run();
	
	indexer.waitDone();

	IndexerProgress p = indexer.progress();
	LOG(Info) << "Indexing of " << p.numFiles << " files in " << p.numDirs << " directories (" << Util::bytesToString(p.bytes) << ") completed in " << p.duration << " seconds";
}