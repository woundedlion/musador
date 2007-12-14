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
#include "C:/projects/Musador/src/Server/test/ServerTest.h"

static ServerTest suite_ServerTest;

static CxxTest::List Tests_ServerTest = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_ServerTest( "C:/projects/Musador/src/Server/test/ServerTest.h", 16, "ServerTest", suite_ServerTest, Tests_ServerTest );

static class TestDescription_ServerTest_testServerIO : public CxxTest::RealTestDescription {
public:
 TestDescription_ServerTest_testServerIO() : CxxTest::RealTestDescription( Tests_ServerTest, suiteDescription_ServerTest, 44, "testServerIO" ) {}
 void runTest() { suite_ServerTest.testServerIO(); }
} testDescription_ServerTest_testServerIO;

#include <cxxtest/Root.cpp>
