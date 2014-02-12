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

	std::string id1;
	int64_t id2 = 0;
	int m1 = 0;
	std::string m2;
	std::wstring m3;
};

struct TestReadWrite
{
	TestReadWrite()
	{}

	bool operator==(const TestReadWrite& r)
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
			;
	}

	template <typename T>
	bool equals(T t1, T t2)
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

	STORM_SERIALIZE(m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11);
};

struct TestNested {
	TestNested(TestEntityAutoKey& m1, TestEntityCompositeKey m2, TestEntityAutoKey *m3, TestEntityAutoKey *m4) :
	m1(m1), m2(m2), m3(m3), m4(m4),
	m5({ 2, 4, 6, 8, 10 }),
	m6({ { "one", "one" }, { "two", "two" } }),
	m7({ { "three", 3 }, { "four", 4 }, { "negative four", -4 } }),
	m8({ { "abc", { 'a', 'b', 'c' } }, { "def", { 'd', 'e', 'f' } } }),
	m9({ { "foo", { TestReadWrite(), TestReadWrite() } } })
	{}

	TestEntityAutoKey& m1;
	TestEntityCompositeKey m2;
	TestEntityAutoKey *m3;
	TestEntityAutoKey *m4;
	std::vector<int> m5;
	std::map<std::string, std::string> m6;
	std::map<std::string, int> m7;
	std::map<std::string, std::vector<char>> m8;
	std::map<std::string, std::vector<TestReadWrite>> m9;

	STORM_SERIALIZE(m1, m2, m3, m4, m5, m6, m7, m9)
};
