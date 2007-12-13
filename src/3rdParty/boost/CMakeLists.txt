PROJECT(boost)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

FILE (GLOB fs_srcs libs/filesystem/src/*.cpp)
SOURCE_GROUP(filesystem FILES ${fs_srcs})

FILE (GLOB signals_srcs libs/signals/src/*.cpp)
SOURCE_GROUP(signals FILES ${signals_srcs})

FILE (GLOB thread_srcs libs/thread/src/*.cpp)
SOURCE_GROUP(thread FILES ${thread_srcs})

ADD_LIBRARY(${PROJECT_NAME} ${fs_srcs} ${signals_srcs} ${thread_srcs})

ADD_DEFINITIONS(-DBOOST_THREAD_BUILD_LIB)
