INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

FILE (GLOB fs_srcs regex/filesystem/*.hpp libs/filesystem/src/*.cpp)
SOURCE_GROUP(filesystem FILES ${fs_srcs})

FILE (GLOB signals_srcs regex/signals/*.hpp libs/signals/src/*.cpp)
SOURCE_GROUP(signals FILES ${signals_srcs})

FILE (GLOB thread_srcs regex/thread/*.hpp libs/thread/src/*.cpp)
SOURCE_GROUP(thread FILES ${thread_srcs})

FILE (GLOB regex_srcs boost/regex/*.hpp libs/regex/src/*.cpp )
SOURCE_GROUP(regex FILES ${regex_srcs})

ADD_LIBRARY(Boost.Filesystem ${fs_srcs})
ADD_LIBRARY(Boost.Signals ${signals_srcs})
ADD_LIBRARY(Boost.Thread ${thread_srcs})
ADD_LIBRARY(Boost.Regex ${regex_srcs})

ADD_DEFINITIONS(-DBOOST_THREAD_BUILD_LIB)
