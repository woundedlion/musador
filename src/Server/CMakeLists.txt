PROJECT(Server)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

FILE(GLOB srcs src/*.cpp)
FILE(GLOB hdrs *.h)

ADD_LIBRARY(${PROJECT_NAME} ${hdrs} ${srcs})

ADD_DEFINITIONS(-DUNICODE -D_UNICODE -DBOOST_REGEX_MATCH_EXTRA)

SET(libs
    BoostDateTime
	BoostThread
	BoostRegex
	BoostFilesystem
	BoostSystem
	IO
	Utilities
	Logger
	Network
    UI
)
BUILD_UNIT_TEST(${libs})
