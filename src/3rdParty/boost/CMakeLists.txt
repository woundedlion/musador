INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

FILE (GLOB_RECURSE date_time_srcs boost/date_time/*.hpp libs/date_time/src/*.cpp )
ADD_LIBRARY(BoostDateTime ${date_time_srcs})

FILE (GLOB fs_srcs boost/filesystem/*.hpp libs/filesystem/src/*.cpp)
ADD_LIBRARY(BoostFilesystem ${fs_srcs})
ADD_DEPENDENCIES(BoostFilesystem Boost.System)

FILE (GLOB signals_srcs boost/signals/*.hpp libs/signals/src/*.cpp)
ADD_LIBRARY(BoostSignals ${signals_srcs})

FILE (GLOB thread_srcs boost/thread/*.hpp libs/thread/src/*.cpp libs/thread/src/win32/*.cpp)
ADD_LIBRARY(BoostThread ${thread_srcs})

FILE (GLOB regex_srcs boost/regex/*.hpp boost/regex/v4/*.hpp libs/regex/src/*.cpp )
ADD_LIBRARY(BoostRegex ${regex_srcs})

FILE (GLOB serialization_srcs boost/serialization/*.hpp libs/serialization/src/*.cpp )
ADD_LIBRARY(BoostSerialization ${serialization_srcs})

FILE (GLOB system_srcs boost/system/*.hpp libs/system/src/*.cpp )
ADD_LIBRARY(BoostSystem ${system_srcs})

ADD_DEFINITIONS(-DBOOST_THREAD_BUILD_LIB -DBOOST_REGEX_MATCH_EXTRA)