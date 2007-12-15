/* Generated file, do not edit */

#ifndef CXXTEST_RUNNING
#define CXXTEST_RUNNING
#endif

#define _CXXTEST_HAVE_STD
#define _CXXTEST_HAVE_EH
#include <cxxtest/TestListener.h>
#include <cxxtest/TestTracker.h>
#include <cxxtest/TestRunner.h>
#include <cxxtest/RealDescriptions.h>
#include <cxxtest/ErrorPrinter.h>

int main() {
 return CxxTest::ErrorPrinter().run();
}
#include "C:/projects/Musador/src/Database/test/DatabaseTest.h"

static DatabaseTest suite_DatabaseTest;

static CxxTest::List Tests_DatabaseTest = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_DatabaseTest( "C:/projects/Musador/src/Database/test/DatabaseTest.h", 10, "DatabaseTest", suite_DatabaseTest, Tests_DatabaseTest );

static class TestDescription_DatabaseTest_testSqliteCreate : public CxxTest::RealTestDescription {
public:
 TestDescription_DatabaseTest_testSqliteCreate() : CxxTest::RealTestDescription( Tests_DatabaseTest, suiteDescription_DatabaseTest, 36, "testSqliteCreate" ) {}
 void runTest() { suite_DatabaseTest.testSqliteCreate(); }
} testDescription_DatabaseTest_testSqliteCreate;

static class TestDescription_DatabaseTest_testSqliteInsert : public CxxTest::RealTestDescription {
public:
 TestDescription_DatabaseTest_testSqliteInsert() : CxxTest::RealTestDescription( Tests_DatabaseTest, suiteDescription_DatabaseTest, 92, "testSqliteInsert" ) {}
 void runTest() { suite_DatabaseTest.testSqliteInsert(); }
} testDescription_DatabaseTest_testSqliteInsert;

static class TestDescription_DatabaseTest_testSqliteUpdate : public CxxTest::RealTestDescription {
public:
 TestDescription_DatabaseTest_testSqliteUpdate() : CxxTest::RealTestDescription( Tests_DatabaseTest, suiteDescription_DatabaseTest, 162, "testSqliteUpdate" ) {}
 void runTest() { suite_DatabaseTest.testSqliteUpdate(); }
} testDescription_DatabaseTest_testSqliteUpdate;

static class TestDescription_DatabaseTest_testSqliteDelete : public CxxTest::RealTestDescription {
public:
 TestDescription_DatabaseTest_testSqliteDelete() : CxxTest::RealTestDescription( Tests_DatabaseTest, suiteDescription_DatabaseTest, 247, "testSqliteDelete" ) {}
 void runTest() { suite_DatabaseTest.testSqliteDelete(); }
} testDescription_DatabaseTest_testSqliteDelete;

#include <cxxtest/Root.cpp>
