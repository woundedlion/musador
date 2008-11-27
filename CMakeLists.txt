PROJECT(Musador)

INCLUDE_DIRECTORIES(	${CMAKE_SOURCE_DIR}/src 
						${CMAKE_SOURCE_DIR}/src/3rdParty
						${CMAKE_SOURCE_DIR}/src/3rdParty/taglib
						${CMAKE_SOURCE_DIR}/src/3rdParty/taglib/toolkit
						${CMAKE_SOURCE_DIR}/src/3rdParty/boost
						${CMAKE_SOURCE_DIR}/src/3rdParty/cxxtest
)
	
SOURCE_GROUP("Header Files" REGULAR_EXPRESSION ".*\\.h")

SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin )
ADD_DEFINITIONS( -D_CRT_SECURE_NO_WARNINGS -D_SCL_SECURE_NO_WARNINGS -DWIN32_LEAN_AND_MEAN -DBOOST_ALL_NO_LIB)

ENABLE_TESTING()

SET(cxxTestGenScript ${CMAKE_SOURCE_DIR}/src/3rdParty/cxxtest/cxxtestgen.pl)
MACRO(genCxxTest testIn testOut)
	MESSAGE("Generating test: ${testOut} from ${testIn}")
	EXEC_PROGRAM(perl ARGS -w ${cxxTestGenScript} --error-printer -o ${testOut} ${testIn})
ENDMACRO(genCxxTest)

MACRO(cxxTestLink)	
    FILE(GLOB_RECURSE testHdrs test/*.h)
    FOREACH(hdr ${testHdrs})
        SET(testSrcs "")
	GET_FILENAME_COMPONENT(base ${hdr} NAME_WE)
	SET(src ${CMAKE_CURRENT_BINARY_DIR}/${base}.cpp)

	IF (NOT EXISTS ${src})
	    genCxxTest(${hdr} ${src})
	ENDIF(NOT EXISTS ${src})		
	ADD_CUSTOM_COMMAND( OUTPUT ${src} 
	                    COMMAND perl -w ${cxxTestGenScript} --error-printer -o ${src} ${hdr}
	                    DEPENDS ${hdr}
                           )
	SET(testExe Test${base})

        ADD_EXECUTABLE(${testExe} ${src} ${hdr})
	TARGET_LINK_LIBRARIES(${testExe} ${PROJECT_NAME} ${ARGV})
	ADD_DEPENDENCIES(${testExe} ${PROJECT_NAME} ${ARGV})
	ADD_TEST(${testExe} ${EXECUTABLE_OUTPUT_PATH}/${testExe}) 
    ENDFOREACH(hdr)	
ENDMACRO(cxxTestLink)

MACRO(ADD_FLAG where flag)
	SET(${where} "${${where}} ${flag}")
ENDMACRO(ADD_FLAG)

ADD_FLAG(CMAKE_C_FLAGS_DEBUG /MTd)
ADD_FLAG(CMAKE_C_FLAGS_RELEASE /MT)
ADD_FLAG(CMAKE_CXX_FLAGS_DEBUG /MTd)
ADD_FLAG(CMAKE_CXX_FLAGS_RELEASE /MT)

ADD_SUBDIRECTORY(src/3rdParty/boost)
ADD_SUBDIRECTORY(src/3rdParty/sqlite)
ADD_SUBDIRECTORY(src/3rdParty/taglib)
ADD_SUBDIRECTORY(src/Config)
ADD_SUBDIRECTORY(src/Database)
ADD_SUBDIRECTORY(src/Indexer)
ADD_SUBDIRECTORY(src/IO)
ADD_SUBDIRECTORY(src/Librarian)
ADD_SUBDIRECTORY(src/Protocol)
ADD_SUBDIRECTORY(src/GUI)
ADD_SUBDIRECTORY(src/Logger)
ADD_SUBDIRECTORY(src/Network)
ADD_SUBDIRECTORY(src/Server)
ADD_SUBDIRECTORY(src/UI)
ADD_SUBDIRECTORY(src/Utilities)


