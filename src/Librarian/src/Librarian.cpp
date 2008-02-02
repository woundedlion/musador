#include "boost/filesystem.hpp"
#include "Librarian.h"
#include "Server/HTTPConnection.h"
#include "Network/Network.h"
#include "Logger/Logger.h"

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

	ServerConfig& serverCfg = Config::instance()->server;

	this->server.start();

	for (ServerConfig::SiteCollection::const_iterator iter = serverCfg.sites.begin(); 
		iter != serverCfg.sites.end(); ++iter)
	{
		sockaddr_in ep = {0};
		ep.sin_family = AF_INET;
		ep.sin_addr.s_addr = ::inet_addr(iter->addr.c_str());
		ep.sin_port = ::htons(iter->port);
		this->server.acceptConnections<HTTPConnection>(ep);
	}

	this->waitForStop();
	this->server.stop();
	this->server.waitForStop();

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