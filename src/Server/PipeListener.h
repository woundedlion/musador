#ifndef PIPE_LISTENER_H_BD0CAAF7_87FA_46e9_9917_0AC6E4B3734D
#define PIPE_LISTENER_H_BD0CAAF7_87FA_46e9_9917_0AC6E4B3734D

#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

#include "Listener.h"

namespace Musador
{
	class ConnectionCtx;

	class PipeListener : public Listener, public boost::enable_shared_from_this<PipeListener>
	{
	public:

		PipeListener(const std::wstring& name);

		~PipeListener();

		void beginAccept(EventHandler handler, boost::any tag = NULL);

		void close();

		HANDLE getPipe();

	protected:

		std::wstring name;
		HANDLE pipe;

	};

	inline HANDLE PipeListener::getPipe()
	{
		return this->pipe;
	}
}


#endif