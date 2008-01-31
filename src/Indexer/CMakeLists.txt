PROJECT(Indexer)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

FILE(GLOB srcs src/*.cpp)
FILE(GLOB hdrs *.h)
SET(libs 
		Boost.Signals
		Boost.Thread
		Boost.Filesystem
		Utilities
		Database
		Logger
		tag
)

ADD_LIBRARY(${PROJECT_NAME} ${hdrs} ${srcs})

ADD_DEFINITIONS(-DUNICODE -D_UNICODE -DBUILD_TAGLIB)
ADD_DEPENDENCIES(${PROJECT_NAME} ${libs})

cxxTestLink(${libs})