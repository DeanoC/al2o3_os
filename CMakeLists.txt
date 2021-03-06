cmake_minimum_required(VERSION 3.12.4)
get_directory_property(hasParent PARENT_DIRECTORY)
if(NOT hasParent)
	option(unittests "unittests" OFF)
	get_filename_component(_PARENT_DIR ${CMAKE_CURRENT_SOURCE_DIR} DIRECTORY)
	set_property(GLOBAL PROPERTY GLOBAL_FETCHDEPS_BASE ${_PARENT_DIR}/al2o3 )
	include(FetchContent)
	FetchContent_Declare( al2o3 GIT_REPOSITORY https://github.com/DeanoC/al2o3 GIT_TAG master )
	FetchContent_GetProperties(al2o3)
	if(NOT al2o3_POPULATED)
		FetchContent_Populate(al2o3)
		add_subdirectory(${al2o3_SOURCE_DIR} ${al2o3_BINARY_DIR})
	endif()
	INIT_AL2O3(${CMAKE_CURRENT_SOURCE_DIR})
endif ()

SET_MIN_VERSIONS()
set(LibName al2o3_os)
project(${LibName})


set(Interface
		file.h
		file.hpp
		filesystem.h
		filesystem.hpp
		monitordesc.h
		platformevents.hpp
		rect.h
		rect.hpp
		time.h
		)
set(Src
		file.c
		filesystem.cpp
		)
if (WIN32)
	list(APPEND Src windows/filesystem.cpp)
	list(APPEND Src windows/time.c)
endif()

if(APPLE)
	list(APPEND Src apple/filesystem.mm)
	list(APPEND Src apple/time.mm)
endif()

if(APPLE OR UNIX)
	list(APPEND Src posix/filesystem.cpp)
endif()

set(Deps
		al2o3_platform
		al2o3_memory
		al2o3_tinystl)
ADD_LIB(${LibName} "${Interface}" "${Src}" "${Deps}")
if (UNIX)
	target_link_libraries(${LibName} PRIVATE "pthread")
endif ()

set( Tests
		runner.cpp
		test_file.cpp
		test_filesystem.cpp
		)
set( TestDeps
		al2o3_catch2 )
ADD_LIB_TESTS(${LibName} "${Interface}" "${Tests}" "${TestDeps}")
ADD_LIB_TESTVECTORS(${LibName})
