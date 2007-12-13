#ifndef SERVER_MSGS_A8167A71_4E20_466d_8D70_C211158BB00D
#define SERVER_MSGS_A8167A71_4E20_466d_8D70_C211158BB00D

namespace Musador
{
	enum ServerMsgType
	{
		CTL_SHUTDOWN,
		CTL_RECYCLE,
	};

	class ServerMsg
	{
	public:
		ServerMsg(ServerMsgType type) : type(type) {}
		~ServerMsg();
		inline ServerMsgType getType() { return type; }
	private: 
		ServerMsgType type;
	};

	class ServerMsgCtlShutdown : public ServerMsg
	{
	public:

		ServerMsgCtlShutdown() : ServerMsg(CTL_SHUTDOWN) {}

	};

	class ServerMsgCtlRecycle : public ServerMsg
	{
	public:

		ServerMsgCtlRecycle() : ServerMsg(CTL_RECYCLE) {}

	};


}
#endif