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
#include "C:/projects/Musador/src/Utilities/test/UtilTest.h"

static UtilTest suite_UtilTest;

static CxxTest::List Tests_UtilTest = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_UtilTest( "C:/projects/Musador/src/Utilities/test/UtilTest.h", 9, "UtilTest", suite_UtilTest, Tests_UtilTest );

static class TestDescription_UtilTest_testEscapeQuotes : public CxxTest::RealTestDescription {
public:
 TestDescription_UtilTest_testEscapeQuotes() : CxxTest::RealTestDescription( Tests_UtilTest, suiteDescription_UtilTest, 24, "testEscapeQuotes" ) {}
 void runTest() { suite_UtilTest.testEscapeQuotes(); }
} testDescription_UtilTest_testEscapeQuotes;

#include <cxxtest/Root.cpp>
