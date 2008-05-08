PROJECT(Musador)

INCLUDE_DIRECTORIES(	${CMAKE_SOURCE_DIR} 
						${CMAKE_SOURCE_DIR}/3rdParty
						${CMAKE_SOURCE_DIR}/3rdParty/taglib
						${CMAKE_SOURCE_DIR}/3rdParty/taglib/toolkit
						${CMAKE_SOURCE_DIR}/3rdParty/boost
						${CMAKE_SOURCE_DIR}/3rdParty/cxxtest
)
	
SOURCE_GROUP("Header Files" REGULAR_EXPRESSION ".*\\.h")

SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin )
ADD_DEFINITIONS( -D_CRT_SECURE_NO_WARNINGS -D_SCL_SECURE_NO_WARNINGS -DWIN32_LEAN_AND_MEAN -DBOOST_ALL_NO_LIB)

ENABLE_TESTING()

SET(cxxTestGenScript ${CMAKE_SOURCE_DIR}/3rdParty/cxxtest/cxxtestgen.pl)
MACRO(genCxxTest testIn testOut)
	MESSAGE("Generating test: "${testOut})
	EXEC_PROGRAM(perl ARGS -w ${cxxTestGenScript} --error-printer -o ${testOut} ${testIn})
ENDMACRO(genCxxTest)

MACRO(cxxTestLink)
	SET(testExe ${PROJECT_NAME}Test)
	
	FILE(GLOB_RECURSE testHdrs test/*.h)
	FOREACH(hdr ${testHdrs})
		GET_FILENAME_COMPONENT(path ${hdr} PATH)
		GET_FILENAME_COMPONENT(base ${hdr} NAME_WE)
		SET(src ${path}/${base}.cpp)
		IF (NOT EXISTS ${src})
			genCxxTest(${hdr} ${src})
		ENDIF(NOT EXISTS ${src})		
		ADD_CUSTOM_COMMAND(	OUTPUT ${src} 
							COMMAND perl -w ${cxxTestGenScript} --error-printer -o ${src} ${hdr}
							DEPENDS ${hdr}
							)
		SET(testSrcs ${testSrcs} ${src} ${hdr})
	ENDFOREACH(hdr)
	
	ADD_EXECUTABLE(${testExe} ${testSrcs})
	TARGET_LINK_LIBRARIES(${testExe} ${PROJECT_NAME} ${ARGV})
	ADD_DEPENDENCIES(${testExe} ${PROJECT_NAME} ${ARGV})
	ADD_TEST(${testExe} ${EXECUTABLE_OUTPUT_PATH}/${testExe}) 
ENDMACRO(cxxTestLink)

MACRO(ADD_FLAG where flag)
	SET(${where} "${${where}} ${flag}")
ENDMACRO(ADD_FLAG)

ADD_FLAG(CMAKE_C_FLAGS_DEBUG /MTd)
ADD_FLAG(CMAKE_C_FLAGS_RELEASE /MT)
ADD_FLAG(CMAKE_CXX_FLAGS_DEBUG /MTd)
ADD_FLAG(CMAKE_CXX_FLAGS_RELEASE /MT)

ADD_SUBDIRECTORY(3rdParty/boost)
ADD_SUBDIRECTORY(3rdParty/sqlite)
ADD_SUBDIRECTORY(3rdParty/taglib)
ADD_SUBDIRECTORY(Config)
ADD_SUBDIRECTORY(Database)
ADD_SUBDIRECTORY(Indexer)
ADD_SUBDIRECTORY(Librarian)
ADD_SUBDIRECTORY(Protocol)
ADD_SUBDIRECTORY(GUI)
ADD_SUBDIRECTORY(Logger)
ADD_SUBDIRECTORY(Network)
ADD_SUBDIRECTORY(Server)
ADD_SUBDIRECTORY(Utilities)


