PROJECT(Indexer)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

FILE(GLOB srcs src/*.cpp)
FILE(GLOB hdrs *.h)

ADD_LIBRARY(${PROJECT_NAME} ${hdrs} ${srcs})

ADD_DEFINITIONS(-DUNICODE -D_UNICODE -DTAGLIB_STATIC)

SET(libs 
        BoostDateTime
		BoostThread
		BoostFilesystem
        BoostSystem
		Utilities
		Database
		Logger
		IO
		Network
        sqlite
		tag
        UI
)
BUILD_UNIT_TEST(${libs})
