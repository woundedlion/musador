PROJECT(IO)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

FILE(GLOB srcs src/*.cpp)
FILE(GLOB hdrs *.h)
SET(libs
	Boost.Thread
	Boost.Regex
	Utilities
	Logger
	Network
)

ADD_LIBRARY(${PROJECT_NAME} ${hdrs} ${srcs})

ADD_DEFINITIONS(-DUNICODE -D_UNICODE -DBOOST_REGEX_MATCH_EXTRA)
ADD_DEPENDENCIES(${PROJECT_NAME} ${libs})

cxxTestLink(${libs} Logger)