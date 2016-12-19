###############################################################
# Project settings
###############################################################

cmake_minimum_required(VERSION 2.8)
project(DCTL CXX)

###############################################################
# Compiler settings
###############################################################

# use C++17 features
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++1z")# -stdlib=libc++")

# fail hard
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror -pedantic-errors -Wno-shadow -Wno-missing-variable-declarations")

# highest general warning level
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -Wconversion -Wsign-conversion")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Weverything")# -Wno-sign-compare -Wno-sign-conversion -Wno-shorten-64-to-32")

# NOT A DEFECT: no C++98 compatibility
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-c++98-compat -Wno-c++98-compat-pedantic")

# NOT A DEFECT: padding is a diagnostic only (useful to turn on during optimizations)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-padded")

# KNOWN ISSUE: Boost.Test does some macro magic that requires static initialization
# http://hadesmem.googlecode.com/svn-history/r1861/trunk/tests/injector.cpp
# http://stackoverflow.com/a/15708829/819272 for a rationale for this warning by Howard Hinnant
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-global-constructors")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-disabled-macro-expansion")

# TODO: DXP message classes with virtual functions should not be header-only
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-weak-vtables")

# optimizations
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O3 -march=native -mtune=native")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -flto")

#set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++ -lc++abi")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -flto -fuse-ld=gold")

###############################################################
# DEPENDENCIES
###############################################################

set(project_include_dirs ${PROJECT_SOURCE_DIR}/include)
include_directories(${project_include_dirs})

set(EXT_include_dirs ${PROJECT_SOURCE_DIR}/../ext)
include_directories(${EXT_include_dirs})

#----------------
# Boost libraries
#----------------
find_package(Boost 1.60.0 REQUIRED COMPONENTS chrono system timer unit_test_framework)
include_directories(SYSTEM ${Boost_INCLUDE_DIRS})
add_definitions(-D BOOST_ALL_DYN_LINK)
add_definitions(-D BOOST_TEST_MAIN)

# 128-bit integers for Clang + libstdc++
add_definitions(-D __GLIBCXX_TYPE_INT_N_0=__int128)
add_definitions(-D __GLIBCXX_BITSIZE_INT_N_0=128)

###############################################################
# TESTING
###############################################################

include(CTest)          # calls enable_testing()
add_subdirectory(test)  # see CMakeLists.txt in test/ subdirectory
