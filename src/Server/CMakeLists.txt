PROJECT(Server)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

FILE(GLOB srcs src/*.cpp)
FILE(GLOB hdrs *.h)
SET(libs
	Boost.Thread
	Boost.Regex
	Boost.Filesystem
	Utilities
	Logger
	Network
)

ADD_LIBRARY(${PROJECT_NAME} ${hdrs} ${srcs})

ADD_DEFINITIONS(-DUNICODE -D_UNICODE)
ADD_DEPENDENCIES(${PROJECT_NAME} ${libs})

cxxTestLink(${libs})