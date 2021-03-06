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
set(K_PREFIX ${CMAKE_INSTALL_PREFIX})
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
string(TOUPPER ${CMAKE_BUILD_TYPE} uppercase_CMAKE_BUILD_TYPE)

option(USE_X86_32 off)
set(KONOHA_CFLAGS "")
if(USE_X86_32)
set(KONOHA_CFLAGS "-m32")
endif(USE_X86_32)

SET(CMAKE_C_FLAGS         "-Wall ${KONOHA_CFLAGS}")
SET(CMAKE_CXX_FLAGS       "-Wall ${KONOHA_CFLAGS}")
SET(CMAKE_C_FLAGS_RELEASE      "-Wall -O2 -g")
SET(CMAKE_C_FLAGS_DEBUG        "-O0 -g3")
SET(CMAKE_C_FLAGS_COMPRESSED   "-Os -g0")
SET(CMAKE_CXX_FLAGS_RELEASE    "${CMAKE_C_FLAGS_RELEASE}")
SET(CMAKE_CXX_FLAGS_DEBUG      "${CMAKE_C_FLAGS_DEBUG}")
SET(CMAKE_CXX_FLAGS_COMPRESSED "${CMAKE_C_FLAGS_COMPRESSED}")

if(${CMAKE_BUILD_TYPE} STREQUAL "Compressed")
	set(K_NODEBUG 1)
endif()

set(RUNTIME
	src/konoha/konoha2.c
)

set(KONOHA_GC_MODULE "" CACHE STRING "konoha gc module")
if(KONOHA_GC_MODULE)
	set(GC module/${KONOHA_GC_MODULE}gc/${KONOHA_GC_MODULE}gc.c)
else(KONOHA_GC_MODULE)
	set(GC src/gc/gc.c)
endif(KONOHA_GC_MODULE)


set(KONOHA_LOGGER_MODULE "" CACHE STRING "konoha logger module")
if(KONOHA_LOGGER_MODULE)
	if("${KONOHA_LOGGER_MODULE}" MATCHES "logpool")
		set(LOGGER_MODULE_LIBS logpool)
	endif()
	set(LOGGER module/${KONOHA_LOGGER_MODULE}/${KONOHA_LOGGER_MODULE}.c)
else(KONOHA_LOGGER_MODULE)
	set(LOGGER src/logger/logger.c)
endif(KONOHA_LOGGER_MODULE)

set(PARSER
	src/sugar/sugar.c
)

set(KONOHA_VM_MODULE "" CACHE STRING "konoha vm module")
if(KONOHA_VM_MODULE)
	set(VM module/${KONOHA_VM_MODULE}vm/${KONOHA_VM_MODULE}asm.c)
else(KONOHA_VM_MODULE)
	set(VM src/vm/asm.c)
endif(KONOHA_VM_MODULE)

set(PACKAGES
	sugar
	konoha
	konoha.null
	konoha.int
	konoha.float
	konoha.math
	konoha.string
	konoha.bytes
	konoha.array
	konoha.hashmap
	konoha.assignment
	konoha.while
	konoha.class
	konoha.global
	llvm
	konoha.pcre
##	konoha.io
	konoha.i
##	konoha.assert
	posix.process
	posix.file
	posix.fd
##	konoha.socket
	konoha.rope
##	konoha.subproc
##	konoha.mpi
##	konoha.curl
	ijit
##	jansson
	python
	apache
	konoha.import
#	openssl
)

find_library(HAVE_LIBDL NAMES dl)
find_library(HAVE_LIBM  NAMES m)

check_type_size("void *" SIZEOF_VOIDP)
check_type_size(long     SIZEOF_LONG)
check_type_size(int      SIZEOF_INT)
check_type_size(float    SIZEOF_FLOAT)
check_type_size(double   SIZEOF_DOUBLE)

check_include_files(dlfcn.h HAVE_DLFCN_H)
check_include_files(iconv.h HAVE_ICONV_H)

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

if(LOGGER_MODULE_LIBS)
set(ExtraLibs ${ExtraLibs} ${LOGGER_MODULE_LIBS})
endif(LOGGER_MODULE_LIBS)

macro(POP _STR _DELIM _RESULT)
	string(REPLACE ${_DELIM} ";" _tmp ${_STR})
	list(REVERSE _tmp)
	list(GET _tmp 0 ${_RESULT})
endmacro()

# for jenkins coverage test
option(TEST_COVERAGE "Coverage Test" OFF)
if(TEST_COVERAGE)
	include(CTest)
	message(STATUS "TEST_COVERAGE = ${TEST_COVERAGE}")
	add_definitions(-fprofile-arcs -ftest-coverage)
	set(ExtraLibs ${ExtraLibs} -fprofile-arcs -ftest-coverage)
endif(TEST_COVERAGE)

# for .k script test
enable_testing()
set(TEST_BIN ${CMAKE_CURRENT_BINARY_DIR}/konoha)
set(PARENT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

macro(add_konoha_test test_name)
	add_test(${test_name} ${TEST_BIN} ${ARGN})
	set_tests_properties(${test_name} PROPERTIES TIMEOUT 10)
	set_tests_properties(${test_name} PROPERTIES ENVIRONMENT
		"KONOHA_PACKAGEPATH=${CMAKE_CURRENT_BINARY_DIR}/konoha2/package")
endmacro(add_konoha_test test_name)

file(GLOB_RECURSE test_files ${CMAKE_CURRENT_SOURCE_DIR}/test/*.k)
foreach(f ${test_files})
	string(REPLACE // / f ${f})
	string(REPLACE ${CMAKE_CURRENT_SOURCE_DIR}/test/ "" test_name ${f})
	add_konoha_test(${test_name} --test-with ${f})
	if(TEST_IJIT)
		add_konoha_test(ijit:${test_name} -Iijit --test-with ${f})
	endif(TEST_IJIT)
	set_tests_properties(${test_name} PROPERTIES TIMEOUT 10)
endforeach(f ${test_files})

add_library(konoha2 SHARED ${RUNTIME} ${GC} ${LOGGER} ${PARSER} ${VM})
target_link_libraries(konoha2 ${ExtraLibs})
set(ExtraLibs ${ExtraLibs} konoha2)

add_executable(konoha2_bin platform/shell/command.c)
target_link_libraries(konoha2_bin ${ExtraLibs})
set_target_properties(konoha2_bin PROPERTIES OUTPUT_NAME konoha)

# package

macro(PKGNAME _STR _RESULT)
	set(_tmp)
	POP(${_STR} "/" _tmp)
	POP(${_tmp} "." ${_RESULT})
endmacro()

macro(create_package_project _PKGNAME)
	project(${_PKGNAME})
	if(TEST_COVERAGE)
	add_definitions(-fprofile-arcs -ftest-coverage)
	set(PACKAGE_EXTRA_LIBRARY ${PACKAGE_EXTRA_LIBRARY} -fprofile-arcs -ftest-coverage)
	endif(TEST_COVERAGE)
	file(GLOB_RECURSE test_files ${CMAKE_CURRENT_SOURCE_DIR}/test/*.k)
	foreach(f ${test_files})
		string(REPLACE // / f ${f})
		string(REPLACE ${PARENT_SOURCE_DIR}/package/ "" test_name ${f})
		string(REPLACE /test "" test_name ${test_name})
		add_konoha_test(${test_name} --test-with ${f})
		if(TEST_IJIT)
			add_konoha_test(ijit:${test_name} -Iijit --test-with ${f})
		endif(TEST_IJIT)
		if (PACKAGE_ENV)
			set_tests_properties(${test_name} PROPERTIES ENVIRONMENT ${PACKAGE_ENV})
		endif (PACKAGE_ENV)
	endforeach(f ${test_files})
	set(CMAKE_BUILD_TYPE ${KONOHA_BUILD_TYPE})
	PKGNAME(${_PKGNAME} _PACKAGE_NAME)
	set(LIB  "${_PACKAGE_NAME}_glue")
	set(_PACKAGE_VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH})
	set(_PACKAGE_STRING  ${CMAKE_PROJECT_NAME}-${_PACKAGE_VERSION})
	set(_KONOHA_PACKAGE_DIR konoha2/package/${KONOHA_VERSION}/${PROJECT_NAME})
	set(CMAKE_C_FLAGS_${uppercase_CMAKE_BUILD_TYPE}
		"${CMAKE_C_FLAGS_${uppercase_CMAKE_BUILD_TYPE}} ${PACKAGE_C_FLAGS}")
	set(CMAKE_CXX_FLAGS_${uppercase_CMAKE_BUILD_TYPE}
		"${CMAKE_CXX_FLAGS_${uppercase_CMAKE_BUILD_TYPE}} ${PACKAGE_CXX_FLAGS}")
	set(_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}
			${CMAKE_CURRENT_SOURCE_DIR}/include  ${CMAKE_CURRENT_BINARY_DIR}
			${KONOHA_INCLUDE_DIRS}
			${_PACKAGE_INCLUDE_DIR})
	include_directories(${_INCLUDE_DIRS})
	list(LENGTH PACKAGE_SOURCE_CODE LEN)
	if(${LEN} GREATER 0)
		add_library(${LIB} SHARED ${PACKAGE_SOURCE_CODE})
		set_target_properties(${LIB} PROPERTIES PREFIX "")
		target_link_libraries(${LIB} konoha2)
		if(PACKAGE_EXTRA_LIBRARY)
		target_link_libraries(${LIB} ${PACKAGE_EXTRA_LIBRARY})
		endif(PACKAGE_EXTRA_LIBRARY)
		install(TARGETS ${LIB} DESTINATION ${_KONOHA_PACKAGE_DIR})
	endif(${LEN} GREATER 0)
	install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/${PACKAGE_SCRIPT_CODE} DESTINATION ${_KONOHA_PACKAGE_DIR})
	add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_SCRIPT_CODE}
		COMMAND ${CMAKE_COMMAND} -E copy_if_different
		${CMAKE_CURRENT_SOURCE_DIR}/${PACKAGE_SCRIPT_CODE}
		${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_SCRIPT_CODE})
	add_custom_target(copy_${PACKAGE_SCRIPT_CODE} ALL DEPENDS
		${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_SCRIPT_CODE})
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

foreach(dir ${PACKAGES})
	set(dir "${CMAKE_CURRENT_SOURCE_DIR}/package/${dir}")
	if(IS_DIRECTORY ${dir} AND EXISTS ${dir}/CMakeLists.txt)
		add_subdirectory(${dir})
	endif()
endforeach()

macro(add_clib_test test_name)
	add_executable(${test_name} ${ARGN})
	target_link_libraries(${test_name} ${ExtraLibs})
	add_test(Clib/Release/${test_name} ${test_name})
endmacro(add_clib_test sources)

add_clib_test(test_sizeof  ./test/src/test_sizeof.c)
add_clib_test(test_Kmalloc ./test/src/test_Kmalloc.c)
add_clib_test(test_Karray  ./test/src/test_Karray.c)
add_clib_test(test_gc      ./test/src/test_gc.c)
add_clib_test(test_Kwb     ./test/src/test_Kwb.c)
add_clib_test(test_map     ./test/src/test_map.c)
add_clib_test(test_string  ./test/src/test_string.c)
add_clib_test(test_cpp     ./test/src/test_cpp.cpp)

macro(add_builtin_test test_name)
	add_konoha_test(Builtin/${test_name} ${ARGN})
endmacro(add_builtin_test test_name)

add_builtin_test(Release/Logger  --builtin-test=test_logger)
add_builtin_test(Release/Kwb     --builtin-test=test_kwb)
add_builtin_test(Release/Kvproto --builtin-test=test_kvproto)

add_test(Shell/Release/Oneline
	${CMAKE_CURRENT_SOURCE_DIR}/test/test-oneline.sh ${TEST_BIN}
	"System.p(\"hi\");")
add_test(Shell/Release/Multiline
	${CMAKE_CURRENT_SOURCE_DIR}/test/test-multiline.sh ${TEST_BIN}
	${CMAKE_CURRENT_SOURCE_DIR}/test/Release/Fibonacci.k)

install(TARGETS konoha2 konoha2_bin
		RUNTIME DESTINATION bin
		LIBRARY DESTINATION lib${LIB_SUFFIX}
		)

install(DIRECTORY include/ DESTINATION include 
		FILES_MATCHING
		PATTERN "*.h"
		PATTERN ".svn" EXCLUDE
		)

MESSAGE(STATUS)
MESSAGE(STATUS "---------------------------------------------------------------------------" )
MESSAGE(STATUS "CMAKE_BUILD_TYPE = ${uppercase_CMAKE_BUILD_TYPE}")
MESSAGE(STATUS "CMAKE_C_COMPILER = ${CMAKE_C_COMPILER}")
MESSAGE(STATUS "CMAKE_CXX_COMPILER = ${CMAKE_CXX_COMPILER}")
MESSAGE(STATUS "CMAKE_C_FLAGS   = ${CMAKE_C_FLAGS_${uppercase_CMAKE_BUILD_TYPE}}")
MESSAGE(STATUS "CMAKE_CXX_FLAGS = ${CMAKE_CXX_FLAGS_${uppercase_CMAKE_BUILD_TYPE}}")
MESSAGE(STATUS "CMAKE_INSTALL_PREFIX = ${CMAKE_INSTALL_PREFIX}")
MESSAGE(STATUS "Change a value with: cmake -D<Variable>=<Value>" )
MESSAGE(STATUS "---------------------------------------------------------------------------" )
MESSAGE(STATUS)
