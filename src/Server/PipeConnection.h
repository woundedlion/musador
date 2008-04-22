#ifndef PIPE_CONNECTION_A8167A71_4E20_466d_8D70_C211158BB00D
#define PIPE_CONNECTION_A8167A71_4E20_466d_8D70_C211158BB00D

#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include "Connection.h"

namespace Musador
{
	class PipeConnection : public Connection, public boost::enable_shared_from_this<PipeConnection>
	{
	public:

		PipeConnection(const std::wstring& name);

		~PipeConnection();

		void close();

		void beginConnect(EventHandler handler, boost::any tag = NULL);

		void beginRead(boost::any tag = NULL);
		void beginRead(boost::shared_ptr<IOMsgReadComplete> msgRead, boost::any tag = NULL);

		void beginWrite(boost::shared_ptr<IOMsgWriteComplete> msgWrite, 
						boost::any tag = NULL);
		void beginWrite(boost::shared_array<char> data, unsigned int len, boost::any tag = NULL);
		void beginWrite(std::istream& dataStream, boost::any tag = NULL);
		void beginWrite(const std::string& str, boost::any tag = NULL);

		std::string toString();

		std::wstring getName();

		HANDLE getPipe();

		void setPipe(HANDLE pipe);

	protected:

		std::wstring name;
		HANDLE pipe;

	};
}

#endif