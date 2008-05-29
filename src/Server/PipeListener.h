#ifndef PIPE_LISTENER_H_BD0CAAF7_87FA_46e9_9917_0AC6E4B3734D
#define PIPE_LISTENER_H_BD0CAAF7_87FA_46e9_9917_0AC6E4B3734D

#include <boost/shared_ptr.hpp>
#include <boost/scoped_ptr.hpp>
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

		std::wstring getName();

	protected:

		std::wstring name;

	private:

		std::string friendlyName();

                HANDLE evtCreated;
	};
}


#endif