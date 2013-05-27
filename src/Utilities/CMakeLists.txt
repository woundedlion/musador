PROJECT(Utilities)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

FILE(GLOB srcs src/*.c src/*.cpp)
FILE(GLOB hdrs *.h)

ADD_LIBRARY(${PROJECT_NAME} ${hdrs} ${srcs})

ADD_DEFINITIONS(-DUNICODE -D_UNICODE)

SET(libs 
    BoostDateTime
    BoostThread
	BoostSystem
    Logger
    UI
)
BUILD_UNIT_TEST(${libs})
