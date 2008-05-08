INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

FILE (GLOB_RECURSE date_time_srcs boost/date_time/*.hpp libs/date_time/src/*.cpp )
ADD_LIBRARY(Boost.DateTime ${date_time_srcs})

FILE (GLOB fs_srcs boost/filesystem/*.hpp libs/filesystem/src/*.cpp)
ADD_LIBRARY(Boost.Filesystem ${fs_srcs})
ADD_DEPENDENCIES(Boost.Filesystem Boost.System)

FILE (GLOB signals_srcs boost/signals/*.hpp libs/signals/src/*.cpp)
ADD_LIBRARY(Boost.Signals ${signals_srcs})

FILE (GLOB thread_srcs boost/thread/*.hpp libs/thread/src/*.cpp libs/thread/src/win32/*.cpp)
ADD_LIBRARY(Boost.Thread ${thread_srcs})

FILE (GLOB regex_srcs boost/regex/*.hpp boost/regex/v4/*.hpp libs/regex/src/*.cpp )
ADD_LIBRARY(Boost.Regex ${regex_srcs})

FILE (GLOB serialization_srcs boost/serialization/*.hpp libs/serialization/src/*.cpp )
ADD_LIBRARY(Boost.Serialization ${serialization_srcs})

FILE (GLOB system_srcs boost/system/*.hpp libs/system/src/*.cpp )
ADD_LIBRARY(Boost.System ${system_srcs})

ADD_DEFINITIONS(-DBOOST_THREAD_BUILD_LIB)
