PROJECT(Librarian)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

FILE(GLOB srcs src/*.cpp)
FILE(GLOB hdrs *.h)
FILE(GLOB rsrcs res/*.rc res/*.h)
SET(libs 
        BoostDateTime
        BoostFilesystem
        BoostRegex
        BoostSignals
        BoostSystem
        BoostSerialization
        BoostThread
		Config
		Database
		Indexer
        IO
		LibrarianProtocol
		Logger
		Network
		Server
        sqlite
        tag
        UI
		Utilities
)

ADD_EXECUTABLE(${PROJECT_NAME} ${hdrs} ${srcs} ${rsrcs})

ADD_DEFINITIONS(-DUNICODE -D_UNICODE )
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${libs})
ADD_DEPENDENCIES(${PROJECT_NAME} ${libs})
