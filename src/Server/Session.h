#ifndef SESSION_H_EC18643C_6239_4528_9ACE_8E6FFB642CEC
#define SESSION_H_EC18643C_6239_4528_9ACE_8E6FFB642CEC

#include <map>
#include <string>
#include <boost/thread/mutex.hpp>

namespace Musador
{

	typedef boost::mutex Mutex;
	typedef boost::mutex::scoped_lock Guard;
	//////////////////////////////////////////////////////////////////////
	/// Session
	//////////////////////////////////////////////////////////////////////

	class Session 
	{
	public:

		Session();

		virtual ~Session();

		std::string& operator[](const std::string& key);

		void clear();

		std::string getName();

		void setName(const std::string& name);

	protected:

		Mutex lock;
		std::map<std::string,std::string> store;
		std::string name;

	};

}

#endif