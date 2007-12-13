/* Generated file, do not edit */

#ifndef CXXTEST_RUNNING
#define CXXTEST_RUNNING
#endif

#define _CXXTEST_HAVE_STD
#include <cxxtest/TestListener.h>
#include <cxxtest/TestTracker.h>
#include <cxxtest/TestRunner.h>
#include <cxxtest/RealDescriptions.h>
#include <cxxtest/ErrorPrinter.h>

int main() {
 return CxxTest::ErrorPrinter().run();
}
#include "C:/projects/Musador/src/Logger/test/LoggerTest.h"

static LoggerTest suite_LoggerTest;

static CxxTest::List Tests_LoggerTest = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_LoggerTest( "C:/projects/Musador/src/Logger/test/LoggerTest.h", 5, "LoggerTest", suite_LoggerTest, Tests_LoggerTest );

static class TestDescription_LoggerTest_testLoggerConsole : public CxxTest::RealTestDescription {
public:
 TestDescription_LoggerTest_testLoggerConsole() : CxxTest::RealTestDescription( Tests_LoggerTest, suiteDescription_LoggerTest, 28, "testLoggerConsole" ) {}
 void runTest() { suite_LoggerTest.testLoggerConsole(); }
} testDescription_LoggerTest_testLoggerConsole;

static class TestDescription_LoggerTest_testLoggerNull : public CxxTest::RealTestDescription {
public:
 TestDescription_LoggerTest_testLoggerNull() : CxxTest::RealTestDescription( Tests_LoggerTest, suiteDescription_LoggerTest, 83, "testLoggerNull" ) {}
 void runTest() { suite_LoggerTest.testLoggerNull(); }
} testDescription_LoggerTest_testLoggerNull;

#include <cxxtest/Root.cpp>
