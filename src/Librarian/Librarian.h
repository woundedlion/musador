#ifndef LIBRARIAN_F34BA23D_EBB0_4871_89C1_20AB9FDC155D
#define LIBRARIAN_F34BA23D_EBB0_4871_89C1_20AB9FDC155D

#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/shared_ptr.hpp>

#include "Utilities/WindowsService.h"
#include "Indexer/Indexer.h"
#include "Server/Server.h"
#include "Utilities/Console.h"
#include "Config/Config.h"
#include "LibrarianController.h"
#include "Protocol/GUIConnection.h"

using namespace Musador;
namespace Musador
{
	class PipeListener;

	class Librarian : public WindowsService<Librarian>
	{
	friend class WindowsService<Librarian>;
	public:

		Librarian();
		
		~Librarian();

		int run(unsigned long argc, wchar_t * argv[]);

		void index(const std::wstring& outfile,const std::vector<std::wstring>& paths);

		void configDefaults(Config& cfg);

		void enable();

		void disable();

	private:

		void Librarian::onAcceptGUIConnection(boost::shared_ptr<IOMsg> msg, boost::any tag);

		template <typename T>
		void notifyGUI();

		boost::shared_ptr<GUIConnection> gui;
		std::auto_ptr<Server> server;
		LibrarianController controller;

	};

	template <typename T>
	void Librarian::notifyGUI()
	{
		if (NULL != this->gui)
		{
			boost::shared_ptr<T> msg(new T());
			std::stringstream msgData;
			boost::archive::binary_oarchive ar(msgData);
			ar & msg;
			this->gui->beginWrite(msgData);
		}
	}

}

#endif