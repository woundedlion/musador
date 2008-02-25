#ifndef LIBRARIAN_F34BA23D_EBB0_4871_89C1_20AB9FDC155D
#define LIBRARIAN_F34BA23D_EBB0_4871_89C1_20AB9FDC155D

#include "Utilities/WindowsService.h"
#include "Indexer/Indexer.h"
#include "Server/Server.h"
#include "Utilities/Console.h"
#include "Config/Config.h"
#include "LibrarianController.h"

using namespace Musador;
namespace Musador
{

	class Librarian : public WindowsService<Librarian>
	{
	friend class WindowsService<Librarian>;
	public:

		Librarian();
		
		~Librarian();

		int run(unsigned long argc, wchar_t * argv[]);

		void index(const std::wstring& outfile,const std::vector<std::wstring>& paths);

		static void configDefaults(Config& cfg);

	private:

		LibrarianController controller;
	};
}

#endif