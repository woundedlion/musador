#ifndef STREAMEXCEPTION_B82D46CD_EBEF_44f5_841C_6F2120109107
#define STREAMEXCEPTION_B82D46CD_EBEF_44f5_841C_6F2120109107

#include <iostream> 
#include <sstream> 
#include <memory> 
#include <stdexcept>

template<class E>
class StreamException : public std::runtime_error 
{ 
	public: 

		StreamException() : std::runtime_error(""), ss_(std::auto_ptr<std::stringstream> (new std::stringstream())) { }

		~StreamException() throw() { }

		template <typename T> E& operator << (const T & t) { (*ss_) << t; return *static_cast<E*>(this); }

		virtual const char * what() const throw() { s_ = ss_->str(); return s_.c_str(); }

	private: 

		mutable std::auto_ptr<std::stringstream> ss_; 
		mutable std::string s_; 
};

#endif