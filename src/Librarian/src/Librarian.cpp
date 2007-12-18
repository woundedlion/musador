#include "Librarian.h"
#include "Server/HTTPProtocol.h"
#include "Network/Network.h"

using namespace Musador;

Librarian::Librarian() :
WindowsService(L"Musador Librarian"),
server(cfg)
{

}

Librarian::~Librarian()
{
}

int Librarian::run(unsigned long argc, wchar_t * argv[])
{
	this->server.start();
	sockaddr_in ep = {0};
	ep.sin_family = AF_INET;
	ep.sin_addr.s_addr = ::inet_addr("0.0.0.0");
	ep.sin_port = ::htons(5152);
        boost::shared_ptr<HTTPProtocolFactory> protocolFactory(new HTTPProtocolFactory);
        this->server.acceptConnections(protocolFactory,ep);
	this->waitForStop();
	this->server.stop();
	return 0;
}
