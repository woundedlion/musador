#ifndef LIBRARIAN_CONTROLLER_H_2B3977A0_7D74_4785_B61B_EECC28F9D290
#define LIBRARIAN_CONTROLLER_H_2B3977A0_7D74_4785_B61B_EECC28F9D290

#include "Server/Controller.h"

namespace Musador
{

	class LibrarianController : public Controller
	{
	public:

		LibrarianController();

	private:

		bool info(HTTP::Env & env);

	};

}

#endif