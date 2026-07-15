#          Copyright Rein Halbersma 2014-2026.
# Distributed under the Boost Software License, Version 1.0.
#    (See accompanying file LICENSE_1_0.txt or copy at
#          http://www.boost.org/LICENSE_1_0.txt)

cmake_minimum_required(VERSION 3.20)
project(
    xstd
    HOMEPAGE_URL https://github.com/rhalbersma/${PROJECT_NAME}
    LANGUAGES CXX
)

add_library(
    ${PROJECT_NAME} INTERFACE
)

set(project_include_dir
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:$<INSTALL_PREFIX>/include>
)
target_include_directories(
    ${PROJECT_NAME} INTERFACE
    ${project_include_dir}
)

target_compile_features(
    ${PROJECT_NAME} INTERFACE
    cxx_std_23
)

# MSVC has no stable /std:c++23 switch yet; /std:c++23preview is incomplete,
# so track /std:c++latest instead. This overrides whatever CMake's cxx_std_23
# feature would otherwise select for MSVC.
target_compile_options(
    ${PROJECT_NAME} INTERFACE
    $<$<CXX_COMPILER_ID:MSVC>:/std:c++latest>
)

include(CTest)
add_subdirectory(test)
