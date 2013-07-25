PROJECT(LibrarianGUI)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

FILE(GLOB srcs src/*.cpp)
FILE(GLOB hdrs *.h)
FILE(GLOB rsrcs res/*.rc res/*.h)
SET(libs 
        BoostDateTime
		BoostSerialization
        BoostThread
		BoostSystem
		Utilities
		Logger
		LibrarianProtocol
        Network
        UI
        IO
)

ADD_EXECUTABLE(${PROJECT_NAME} WIN32 ${hdrs} ${srcs} ${rsrcs})

ADD_DEFINITIONS(-DUNICODE -D_UNICODE)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${libs})
ADD_DEPENDENCIES(${PROJECT_NAME} ${libs})
