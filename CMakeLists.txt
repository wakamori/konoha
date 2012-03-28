include(CheckTypeSize)
include(CheckIncludeFiles)
include(CheckSymbolExists)
include(CheckLibraryExists)

cmake_minimum_required(VERSION 2.6)

project(konoha2)
set(VERSION_MAJOR "2")
set(VERSION_MINOR "0")
set(VERSION_PATCH "0")

set(PACKAGE_NAME    ${PROJECT_NAME})
set(PACKAGE_VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH})
set(PACKAGE_STRING  ${CMAKE_PROJECT_NAME}-${PACKAGE_VERSION})
set(SO_VERSION      ${VERSION_MAJOR}.${VERSION_MINOR})
set(PACKAGE_BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR})

set(K_CONFIG_ARCH ${CMAKE_SYSTEM})
set(K_CONFIG_OS   ${CMAKE_SYSTEM_NAME})
set(K_CONFIG_CPU  ${CMAKE_SYSTEM_PROCESSOR})

if(UNIX)
	set(K_OSDLLEXT ".so")
endif(UNIX)

if(APPLE)
	set(K_OSDLLEXT ".dylib")
endif(APPLE)

if(CYGWIN)
	set(K_OSDLLEXT ".so")
endif(CYGWIN)

if(MSVC)
	set(K_OSDLLEXT ".dll")
endif(MSVC)

if(MINGW)
	set(K_OSDLLEXT ".dll")
endif(MINGW)

set(INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_CURRENT_BINARY_DIR})
include_directories(${INCLUDE_DIRS})

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE "Release" )
endif()

SET(CMAKE_C_FLAGS         "-Wall")
SET(CMAKE_CXX_FLAGS       "-Wall")
SET(CMAKE_C_FLAGS_RELEASE   "-Wall -O2 -g")
SET(CMAKE_CXX_FLAGS_RELEASE "-Wall -O2 -g")
SET(CMAKE_C_FLAGS_DEBUG     "-O0 -g3")
SET(CMAKE_CXX_FLAGS_DEBUG   "-O0 -g3")
SET(CMAKE_C_FLAGS_SMALL     "-Os -g0")
SET(CMAKE_CXX_FLAGS_SMALL   "-Os -g0")

if(${CMAKE_BUILD_TYPE} STREQUAL "Small")
	set(K_NODEBUG 1)
endif()

set(RUNTIME
	src/konoha/datatype.c
	src/konoha/klib.c
	src/konoha/konoha2.c
)

set(GC
	src/gc/gc.c
)

set(PARSER
	src/sugar/sugar.c
)

set(VM
	src/vm/asm.c
)

set(PACKAGES
	konoha.string
	konoha.float
	konoha.loop
	sugar
	posix.process
)

find_library(HAVE_LIBDL NAMES dl)
find_library(HAVE_LIBM  NAMES m)

check_type_size("void *" SIZEOF_VOIDP)
check_type_size(long     SIZEOF_LONG)
check_type_size(int      SIZEOF_INT)
check_type_size(float    SIZEOF_FLOAT)
check_type_size(double   SIZEOF_DOUBLE)

check_include_files(dlfcn.h HAVE_DLFCN_H)

FIND_FILE(HAS_DOTGIT .git PATHS "${CMAKE_CURRENT_SOURCE_DIR}")
if("${HAS_DOTGIT}" MATCHES ".git")
	find_package(Git)
	if(GIT_EXECUTABLE)
		execute_process(
				COMMAND ${GIT_EXECUTABLE} rev-parse -q --short=8 HEAD
			OUTPUT_VARIABLE GIT_SHA1
				OUTPUT_STRIP_TRAILING_WHITESPACE
				)
	set(K_REVISION "0x${GIT_SHA1}")
	set(K_REVISION_PREFIX "git:")
	endif(GIT_EXECUTABLE)
else()
find_package(Subversion)
if(Subversion_FOUND)
	set(_PATH)
	get_filename_component(_PATH ${CMAKE_CURRENT_SOURCE_DIR} REALPATH)
	Subversion_WC_INFO(${_PATH} ER)
	set(K_REVISION ${ER_WC_REVISION})
	set(K_REVISION_PREFIX "svn:")
endif(Subversion_FOUND)
endif()

add_definitions(-DHAVE_CONFIG_H)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.cmake
		${CMAKE_CURRENT_BINARY_DIR}/config.h)

set(ExtraLibs)
if(HAVE_LIBDL)
set(ExtraLibs ${ExtraLibs} ${HAVE_LIBDL})
endif(HAVE_LIBDL)

add_library(konoha2 SHARED ${RUNTIME} ${GC} ${PARSER} ${VM})
target_link_libraries(konoha2 ${ExtraLibs})
set(ExtraLibs ${ExtraLibs} konoha2)

add_executable(konoha2_bin src/konoha/main.c)
target_link_libraries(konoha2_bin ${ExtraLibs})
set_target_properties(konoha2_bin PROPERTIES OUTPUT_NAME konoha2)

# package

macro(POP _STR _DELIM _RESULT)
	string(REPLACE ${_DELIM} ";" _tmp ${_STR})
	list(REVERSE _tmp)
	list(GET _tmp 0 ${_RESULT})
endmacro()

macro(PKGNAME _STR _RESULT)
	set(_tmp)
	POP(${_STR} "/" _tmp)
	POP(${_tmp} "." ${_RESULT})
endmacro()

macro(create_package_project _PKGNAME)
	project(${_PKGNAME})
	set(CMAKE_BUILD_TYPE ${KONOHA_BUILD_TYPE})
	#set(_PACKAGE_NAME    ${PROJECT_NAME})
	PKGNAME(${_PKGNAME} _PACKAGE_NAME)
	set(LIB  "${_PACKAGE_NAME}_glue")
	set(_PACKAGE_VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH})
	set(_PACKAGE_STRING  ${CMAKE_PROJECT_NAME}-${_PACKAGE_VERSION})
	set(_KONOHA_PACKAGE_DIR konoha2/package/${KONOHA_VERSION}/${PROJECT_NAME})
	set(_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}
			${CMAKE_CURRENT_SOURCE_DIR}/include  ${CMAKE_CURRENT_BINARY_DIR}
			${KONOHA_INCLUDE_DIRS}
			${_PACKAGE_INCLUDE_DIR})
	include_directories(${_INCLUDE_DIRS})
	list(LENGTH PACKAGE_SOURCE_CODE LEN)
	if(${LEN} GREATER 0)
		add_library(${LIB} SHARED ${PACKAGE_SOURCE_CODE})
		set_target_properties(${LIB} PROPERTIES PREFIX "")
		target_link_libraries(${LIB} konoha2 ${PACKAGE_EXTRA_LIBRARY})
		install(TARGETS ${LIB} DESTINATION ${_KONOHA_PACKAGE_DIR})
	endif(${LEN} GREATER 0)
	install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/${PACKAGE_SCRIPT_CODE} DESTINATION ${_KONOHA_PACKAGE_DIR})
endmacro(create_package_project)

macro(add_konoha_package PKGNAME)
	set(__BUILD true)
	foreach(arg ${ARGN})
		if (NOT ${arg})
			set(__BUILD false)
		endif (NOT ${arg})
	endforeach(arg ${ARGN})
	if (__BUILD)
		create_package_project(${PKGNAME})
	endif (__BUILD)
endmacro(add_konoha_package)

#file(GLOB dirs "${CMAKE_CURRENT_SOURCE_DIR}/package/konoha.*")
#foreach(dir ${dirs})
#	add_subdirectory(${dir})
#endforeach()

foreach(dir ${PACKAGES})
	add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/package/${dir})
endforeach()


# test cases
enable_testing()

macro(add_konoha_test scr)
set(TEST_CMD ${CMAKE_CURRENT_SOURCE_DIR}/misc/check_stmt.sh)
set(TEST_DIR ${CMAKE_CURRENT_SOURCE_DIR}/test/parser)
set(TEST_BIN ${CMAKE_CURRENT_BINARY_DIR}/konoha2)
add_test(NAME ${scr} COMMAND ${TEST_CMD} ${TEST_BIN} ${TEST_DIR} ${scr})
add_test(NAME "${scr}_run" COMMAND ${TEST_BIN} "${TEST_DIR}/${scr}.k")
endmacro(add_konoha_test scr)

set(TEST_CODE
		"call"
		"empty"
		"expr"
		"fibo"
		"getter"
		"p"
		"if"
		"if2"
		"return"
		"string"
		"usymbol"
		"var"
		"while"
		"ropestring"
		"20120328_int_call"
)

foreach(arg ${TEST_CODE})
	add_konoha_test(${arg})
endforeach()

add_executable(test_Kmalloc ./test/test_Kmalloc.c)
target_link_libraries(test_Kmalloc ${ExtraLibs})
add_test(test_Kmalloc test_Kmalloc)

add_executable(test_vm_add0 ./test/vm/test_add0.c)
target_link_libraries(test_vm_add0 ${ExtraLibs})
add_test(test_vm_add0 test_vm_add0)

add_executable(test_vm_int ./test/vm/test_int.c)
target_link_libraries(test_vm_int ${ExtraLibs})
add_test(test_vm_int test_vm_int)

add_executable(test_Karray ./test/test_Karray.c)
target_link_libraries(test_Karray ${ExtraLibs})
add_test(test_Karray test_Karray)

add_executable(test_gc ./test/test_gc.c)
target_link_libraries(test_gc ${ExtraLibs})
add_test(test_gc test_gc)

add_executable(test_Kwb ./test/test_Kwb.c)
target_link_libraries(test_Kwb ${ExtraLibs})
add_test(test_Kwb test_Kwb)

add_executable(test_map ./test/test_map.c)
target_link_libraries(test_map ${ExtraLibs})
add_test(test_map test_map)

add_executable(test_string ./test/test_string.c)
target_link_libraries(test_string ${ExtraLibs})
add_test(test_string test_string)

#add_executable(test_loadMethodData ./test/test_loadMethodData.c)
#target_link_libraries(test_loadMethodData ${ExtraLibs})
#add_test(test_loadMethodData test_loadMethodData)
#
#add_executable(test_getMethod ./test/test_getMethod.c)
#target_link_libraries(test_getMethod ${ExtraLibs})
#add_test(test_getMethod test_getMethod)

add_executable(test_sizeof ./test/test_sizeof.c)
add_test(test_sizeof test_sizeof)

install(TARGETS konoha2 konoha2_bin
		RUNTIME DESTINATION bin
		LIBRARY DESTINATION lib
		)

install(DIRECTORY include/ DESTINATION include 
		FILES_MATCHING
		PATTERN "*.h"
		PATTERN ".svn" EXCLUDE
		)
