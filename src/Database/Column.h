#ifndef COLUMN_13B33D06_6101_44ba_A429_9A9FAC650E0B
#define COLUMN_13B33D06_6101_44ba_A429_9A9FAC650E0B

#include <sstream>
#include "Utilities/Util.h"
#include "ResultSet.h"

class ColumnBase
{
public:

	ColumnBase(std::wstring name):
		nameStr(name),
		dirty(false)
	{}

	virtual ~ColumnBase() 
	{}

	inline const std::wstring& name()
	{
		return this->nameStr;
	}

	inline bool isDirty()
	{
		return this->dirty;
	}

	inline void setDirty(bool d)
	{
		this->dirty = d;
	}

	virtual void writeValue(std::wstringstream& out) = 0;

	virtual void extractResult(const ResultSet * r, int iCol) = 0;

private:

	std::wstring nameStr;
	bool dirty;

};

template <typename T>
class Column : public ColumnBase
{
public:

	Column(const std::wstring& name);

	~Column();

	Column<T>& operator=(const T& rhs);	

	bool operator==(const T& rhs);

	inline operator T() { return data; }

	void writeValue(std::wstringstream& out);

	void extractResult(const ResultSet * r, int iCol);

private:

	T data;

};

template <typename T>
Column<T>::Column(const std::wstring& name):
	ColumnBase(name),
	data(T())
{
}

template <typename T>
Column<T>::~Column()
{

}

template <typename T>
Column<T>& Column<T>::operator=(const T& rhs)
{
	this->data = rhs;
	this->setDirty(true);
	return *this;
}

template <typename T>
inline bool Column<T>::operator==(const T& rhs)
{
	return this->data == rhs;
}

// Tempalate specializations


template <typename T>
inline void Column<T>::writeValue(std::wstringstream& out)
{
	out << this->data;
}

template <>
inline void Column<std::string>::writeValue(std::wstringstream& out)
{
	out << L'\'' << Util::escapeQuotes(Util::utf8ToUnicode(this->data.c_str())) << '\'';
}

template <>
inline void Column<char *>::writeValue(std::wstringstream& out)
{
	out << L'\'' << Util::escapeQuotes(Util::utf8ToUnicode(this->data)) << L'\'';
}

template <>
inline void Column<std::wstring>::writeValue(std::wstringstream& out)
{
	out << L'\'' << Util::escapeQuotes(this->data) << L'\'';
}

template <>
inline void Column<wchar_t *>::writeValue(std::wstringstream& out)
{
	out << L'\'' << Util::escapeQuotes(this->data) << L'\'';
}

////////////////////////////////////////////////////////////////////////////////////////////

template <>
inline void Column<std::string>::extractResult(const ResultSet * r, int iCol)
{
	const char * bytes = r->getText(iCol);
	if (NULL != bytes)
		*this = bytes;
}

template <>
inline void Column<std::wstring>::extractResult(const ResultSet * r, int iCol)
{
	const char * bytes = r->getText(iCol);
	if (NULL != bytes)
		*this = Util::utf8ToUnicode(bytes);
}

template <>
inline void Column<char *>::extractResult(const ResultSet * r, int iCol)
{
	const char * bytes = r->getText(iCol);
	if (NULL != bytes)
		::strcpy(this->data,bytes);
}

template <>
inline void Column<wchar_t *>::extractResult(const ResultSet * r, int iCol)
{
	const char * bytes = r->getText(iCol);
	if (NULL != bytes)
	{
		std::wstring wStr = Util::utf8ToUnicode(bytes);
		::wcscpy(this->data, wStr.c_str());
	}
}

template <>
inline void Column<unsigned char *>::extractResult(const ResultSet * r, int iCol)
{
	const unsigned char * bytes = r->getBlob(iCol);
	if (NULL != bytes)
		::memcpy(this->data,bytes,r->getSize(iCol));
}

template <>
inline void Column<unsigned int>::extractResult(const ResultSet * r, int iCol)
{
	*this = static_cast<unsigned int>(r->getInt(iCol));
}

template <>
inline void Column<int>::extractResult(const ResultSet * r, int iCol)
{
	*this = r->getInt(iCol);
}

template <>
inline void Column<long>::extractResult(const ResultSet * r, int iCol)
{
	*this = r->getInt(iCol);
}

template <>
inline void Column<unsigned long>::extractResult(const ResultSet * r, int iCol)
{
	*this = r->getInt(iCol);
}

template <>
inline void Column<double>::extractResult(const ResultSet * r, int iCol)
{
	*this = r->getDouble(iCol);
}

template <>
inline void Column<__int64>::extractResult(const ResultSet * r, int iCol)
{
	*this = r->getInt64(iCol);
}

#endif