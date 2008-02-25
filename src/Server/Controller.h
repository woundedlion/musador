#ifndef CONTROLLER_H_2B3977A0_7D74_4785_B61B_EECC28F9D290
#define CONTROLLER_H_2B3977A0_7D74_4785_B61B_EECC28F9D290

#include <map>
#include "boost/function.hpp"

namespace Musador
{
	namespace HTTP
	{
		class Env;
	}

	class Controller
	{
	public:

		typedef boost::function1<bool, HTTP::Env &> Handler;

		virtual ~Controller() {}

		virtual bool exec(HTTP::Env& env);	

		void addHandler(const std::string& requestURI, Handler handler);

	private:

		typedef std::map<std::string,Handler> HandlerMap;

		HandlerMap handlers;

	};

}

#endif