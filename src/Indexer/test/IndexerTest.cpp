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
#include "C:/projects/Musador/src/Indexer/test/IndexerTest.h"

static IndexerTest suite_IndexerTest;

static CxxTest::List Tests_IndexerTest = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_IndexerTest( "C:/projects/Musador/src/Indexer/test/IndexerTest.h", 8, "IndexerTest", suite_IndexerTest, Tests_IndexerTest );

static class TestDescription_IndexerTest_testIndexer : public CxxTest::RealTestDescription {
public:
 TestDescription_IndexerTest_testIndexer() : CxxTest::RealTestDescription( Tests_IndexerTest, suiteDescription_IndexerTest, 81, "testIndexer" ) {}
 void runTest() { suite_IndexerTest.testIndexer(); }
} testDescription_IndexerTest_testIndexer;

#include <cxxtest/Root.cpp>
