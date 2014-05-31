#pragma once

#include <cstdint>
#include "Database/Storm.h"
#include "Utilities/Util.h"

class TestEntityAutoKey
{
public:

	TestEntityAutoKey() :
		id(0),
		m1(0)
	{}

	template <class Archive>
	void serialize(Archive& ar, unsigned int)
	{
		ar & STORM_AUTO_PKEY_NVP(id);
		ar & STORM_NVP(m1);
		ar & STORM_NVP(m2);
		ar & STORM_NVP(m3);
	}

	bool operator==(const TestEntityAutoKey& r) const
	{
		return r.id == id
			&& r.m1 == m1
			&& r.m2 == m2
			&& r.m3 == m3;
	}

	STORM_TABLE(test)

	int64_t id;
	int m1;
	std::string m2;
	std::wstring m3;
};

class TestEntityCompositeKey
{
public:

	TestEntityCompositeKey()
	{}

	STORM_TABLE(test_composite_key)

	template <class Archive>
	void serialize(Archive& ar, unsigned int)
	{
		ar & STORM_COMP_PKEY_NVP(id1);
		ar & STORM_COMP_PKEY_NVP(id2);
		ar & STORM_NVP(m1);
		ar & STORM_NVP(m2);
		ar & STORM_NVP(m3);
	}

	bool operator==(const TestEntityCompositeKey& r) const
	{
		return r.id1 == id1
			&& r.id2 == id2
			&& r.m1 == m1
			&& r.m2 == m2
			&& r.m3 == m3;
	}

	std::string id1;
	int64_t id2 = 0;
	int m1 = 0;
	std::string m2;
	std::wstring m3;
};

struct TestReadWrite
{
	TestReadWrite() :
	m12({ { "foo", 999 }, { "qux", -999 } }),
	m13({ "biz", "boz", "baz" }),
	m14({ { "bar", { 1, 2, 3, 4 } }, { "baz", { 123, 456, 678 } } })
	{}

	bool operator==(const TestReadWrite& r) const
	{
		return r.m1 == m1
			&& r.m2 == m2
			&& r.m3 == m3
			&& r.m4 == m4
			&& r.m5 == m5
			&& equals(r.m6, m6)
			&& equals(r.m7, m7)
			&& r.m8 == m8
			&& r.m9 == m9
			&& r.m10 == m10
			&& r.m11 == m11
			&& r.m12 == m12
			&& r.m13 == m13
			&& r.m14 == m14
			;
	}

	template <typename T>
	static bool equals(const T& t1, const T& t2)
	{
		return fabs(t1 - t2) < std::numeric_limits<T>::epsilon();
	}

	bool m1 = false;
	int m2 = 0;
	int64_t m3 = 0;
	uint32_t m4 = 0;
	uint64_t m5 = 0;
	float m6 = 0;
	double m7 = 0;
	std::string m8;
	std::wstring m9;
	char m10 = 'a';
	char m11 = 'b';
	std::map<std::string, int> m12;
	std::vector<std::string> m13;
	std::map<std::string, std::vector<uint64_t>> m14;

	STORM_SERIALIZE(m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11, m12, m13, m14);
};

struct TestNested {
	TestNested(TestEntityAutoKey& m1, TestEntityCompositeKey m2) :
	m1(m1), 
	m2(m2),
	m3({ 2, 4, 6, 8, 10 }),
	m4({ { "one", "one" }, { "two", "two" } }),
	m5({ { "three", 3 }, { "four", 4 }, { "negative four", -4 } }),
	m6({ { "abc", { 'a', 'b', 'c' } }, { "def", { 'd', 'e', 'f' } } }),
	m7({ { "foo", { TestReadWrite(), TestReadWrite() } } })
	{}

	bool operator==(const TestNested&)
	{
		return m1 == m1
			&& m2 == m2
			&& m3 == m3
			&& m4 == m4
			&& m5 == m5
			&& m6 == m6
			&& m7 == m7;
	}

	TestEntityAutoKey& m1;
	TestEntityCompositeKey m2;
	std::vector<int> m3;
	std::map<std::string, std::string> m4;
	std::map<std::string, int> m5;
	std::map<std::string, std::vector<char>> m6;
	std::map<std::string, std::vector<TestReadWrite>> m7;

	STORM_SERIALIZE(m1, m2, m3, m4, m5, m6, m7)
};
