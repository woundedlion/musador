#include "boost/bind.hpp"
#include "PipeConnection.h"
#include "Utilities/Util.h"
#include "Proactor.h"

#include "Logger/Logger.h"
#define LOG_SENDER L"I/O"
using namespace Musador;

PipeConnection::PipeConnection(const std::wstring& name, HANDLE pipe) :
name(name),
pipe(pipe)
{
}

PipeConnection::~PipeConnection()
{
	this->close();
}

void 
PipeConnection::close()
{
	if (NULL != this->pipe)
	{
		::CloseHandle(this->pipe);
		this->pipe = NULL;
	}
}

void 
PipeConnection::beginRead()
{
	Proactor::instance()->beginRead(this->shared_from_this(), boost::bind(&ConnectionProcessor::post,this->ctx->processor,_1,_2));
}

void 
PipeConnection::beginRead(boost::shared_ptr<IOMsgReadComplete> msgRead)
{
	Proactor::instance()->beginRead(this->shared_from_this(), boost::bind(&ConnectionProcessor::post,this->ctx->processor,_1,_2), msgRead);
}

inline
void 
PipeConnection::beginWrite(EventHandler handler, 
							 boost::shared_ptr<IOMsgWriteComplete> msgWrite, 
							 boost::any tag /* = NULL */)
{
	Proactor::instance()->beginWrite(this->shared_from_this(), handler, msgWrite, tag);
}


void 
PipeConnection::beginWrite(boost::shared_array<char> data, unsigned int len)
{
	Proactor::instance()->beginWrite(this->shared_from_this(), boost::bind(&ConnectionProcessor::post,this->ctx->processor,_1,_2), data, len);
}

void 
PipeConnection::beginWrite(std::istream& dataStream)
{
	boost::shared_array<char> data(new char[IOMsgWriteComplete::MAX]);
	dataStream.read(data.get(),IOMsgWriteComplete::MAX);
	int len = dataStream.gcount();
	Proactor::instance()->beginWrite(this->shared_from_this(), boost::bind(&ConnectionProcessor::post,this->ctx->processor,_1,_2), data, len);
}

void 
PipeConnection::beginWrite(const std::string& str)
{
	unsigned int len = str.size();
	boost::shared_array<char> data(new char[len]);
	str.copy(data.get(), len);
	Proactor::instance()->beginWrite(this->shared_from_this(), boost::bind(&ConnectionProcessor::post,this->ctx->processor,_1,_2), data, len);
}

std::string 
PipeConnection::toString()
{
	return Util::unicodeToUtf8(this->name);
}

HANDLE 
PipeConnection::getPipe()
{
	return this->pipe;
}

void 
PipeConnection::setPipe(HANDLE pipe)
{
	this->pipe = pipe;
}
