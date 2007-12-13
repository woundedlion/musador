PROJECT(Indexer)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

FILE(GLOB srcs src/*.cpp)
FILE(GLOB hdrs *.h)
SET(libs 
		boost
		Utilities
		Database
		tag
)

ADD_LIBRARY(${PROJECT_NAME} ${hdrs} ${srcs})

ADD_DEFINITIONS(-DUNICODE -D_UNICODE -DBUILD_TAGLIB)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${libs})
ADD_DEPENDENCIES(${PROJECT_NAME} ${libs})

cxxTestLink(${libs} Logger)