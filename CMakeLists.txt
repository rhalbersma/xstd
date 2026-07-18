#          Copyright Rein Halbersma 2014-2026.
# Distributed under the Boost Software License, Version 1.0.
#    (See accompanying file LICENSE_1_0.txt or copy at
#          http://www.boost.org/LICENSE_1_0.txt)

cmake_minimum_required(VERSION 3.28)
project(
    xstd
    VERSION 0.1.0
    DESCRIPTION "Extensions to the C++ Standard Library"
    HOMEPAGE_URL "https://github.com/rhalbersma/xstd"
    LANGUAGES CXX
)

include(GNUInstallDirs)

add_library(
    ${PROJECT_NAME} INTERFACE
)
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

# No target_include_directories: the HEADERS file set below adds its base
# directory to the build interface, and install(TARGETS ... FILE_SET) adds
# the install destination to the install interface.
target_compile_features(
    ${PROJECT_NAME} INTERFACE
    cxx_std_23
)

target_sources(
    ${PROJECT_NAME} INTERFACE
    FILE_SET HEADERS
    BASE_DIRS include
    FILES
        include/xstd/array.hpp
        include/xstd/cstdlib.hpp
        include/xstd/type_traits.hpp
        include/xstd/utility.hpp
)

# Tests (and their Boost.Test dependency) only exist when xstd itself is
# being developed. Consumers vendoring this directory via add_subdirectory
# or FetchContent get just the header-only target, with no BUILD_TESTING
# workaround required on their side.
if(PROJECT_IS_TOP_LEVEL)
    include(CTest)
    if(BUILD_TESTING)
        add_subdirectory(test)
    endif()
endif()

install(
    TARGETS ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}Targets
    FILE_SET HEADERS
)
install(
    EXPORT ${PROJECT_NAME}Targets
    NAMESPACE ${PROJECT_NAME}::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)

# Package config + version files, so consumers can locate the installed
# package with find_package(xstd CONFIG). ARCH_INDEPENDENT because a
# header-only library has no architecture-specific binaries.
include(CMakePackageConfigHelpers)
configure_package_config_file(
    cmake/${PROJECT_NAME}Config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
    COMPATIBILITY SameMinorVersion
    ARCH_INDEPENDENT
)
install(
    FILES
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)
