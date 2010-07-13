PROJECT(Database)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

FILE(GLOB srcs src/*.cpp)
FILE(GLOB hdrs *.h)

ADD_LIBRARY(${PROJECT_NAME} ${hdrs} ${srcs})

ADD_DEFINITIONS(-DUNICODE -D_UNICODE)

SET(libs
    BoostDateTime
	BoostThread
	sqlite
	Utilities
	Logger
    UI
)
cxxTestLink(${libs})
