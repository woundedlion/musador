#ifndef HTTP_PROTOCOL_B85D188B_14AE_402b_8977_BEB491AAB474
#define HTTP_PROTOCOL_B85D188B_14AE_402b_8977_BEB491AAB474

#include "Protocol.h"
#include "HTTP.h"

namespace Musador
{

	class HTTPProtocol : public Protocol
	{
	public:

                enum StateType
                {
                    RECV_REQ_HEADER_STATE,
                    RECV_REQ_DATA_STATE,
                };

		HTTPProtocol();
		
		~HTTPProtocol();

		void operator<<(boost::shared_ptr<IOMsgReadComplete> msgRead);

	private:
    
            StateType state;
            void stateRecvReqHeader(boost::shared_ptr<IOMsgReadComplete> msgRead);
            void stateRecvReqData(boost::shared_ptr<IOMsgReadComplete> msgRead);
        };

        class HTTPProtocolFactory : public ConcreteFactory<Protocol,HTTPProtocol> { };
}
#endif