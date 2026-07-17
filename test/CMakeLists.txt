#          Copyright Rein Halbersma 2014-2026.
# Distributed under the Boost Software License, Version 1.0.
#    (See accompanying file LICENSE_1_0.txt or copy at
#          http://www.boost.org/LICENSE_1_0.txt)

# strict -std=c++23 rather than the default gnu++23 dialect
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(xstd_test_options INTERFACE)
option(XSTD_WARNINGS_AS_ERRORS "Treat warnings as errors in xstd tests" ${PROJECT_IS_TOP_LEVEL})

# clang-cl reports CXX_COMPILER_ID Clang but only understands its MSVC-style
# driver flags, not GNU-style ones like -pedantic-errors.
if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
    set(xstd_is_clang_cl ON)
else()
    set(xstd_is_clang_cl OFF)
endif()

set(cxx_compile_definitions
    BOOST_CONFIG_SUPPRESS_OUTDATED_MESSAGE  # https://github.com/boostorg/config/issues/207
    BOOST_ALL_NO_LIB
    BOOST_TEST_MAIN
    $<$<CXX_COMPILER_ID:MSVC>:
        _SCL_SECURE_NO_WARNINGS
    >
)

set(cxx_compile_options_warnings
    $<$<CXX_COMPILER_ID:MSVC>:
        /W4
        /permissive-
    >
    $<$<CXX_COMPILER_ID:Clang>:
        -Weverything
        -Wno-unknown-warning-option         # forward compatibility for new Clang warnings
        -Wno-c++98-compat
        -Wno-c++98-compat-pedantic
        -Wno-disabled-macro-expansion       # triggered by Boost.Test
        -Wno-global-constructors            # triggered by Boost.Test
        -Wno-used-but-marked-unused         # triggered by Boost.Test
        -Wno-c2y-extensions                 # triggered by Boost.Test's use of __COUNTER__
        -Wno-lifetime-safety-lifetimebound-violation  # can't verify chained operator<< returns ostr
    >
    $<$<CXX_COMPILER_ID:GNU>:
        -Wall
        -Wextra
        -Wpedantic
        -Wconversion
        -Wshadow
        -Wsign-compare
        -Wsign-conversion
        -Wsign-promo
    >
    $<$<AND:$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>,$<NOT:$<BOOL:${xstd_is_clang_cl}>>>:
        -pedantic-errors
    >
)

target_compile_definitions(
    xstd_test_options INTERFACE
    ${cxx_compile_definitions}
)
target_compile_options(
    xstd_test_options INTERFACE
    ${cxx_compile_options_warnings}
    $<$<AND:$<BOOL:${XSTD_WARNINGS_AS_ERRORS}>,$<CXX_COMPILER_ID:MSVC>>:/WX>
    $<$<AND:$<BOOL:${XSTD_WARNINGS_AS_ERRORS}>,$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>>:-Werror>
)
target_include_directories(
    xstd_test_options INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Each public header is compiled as its own translation unit (linked against
# the same warning flags as the unit tests), proving it is self-sufficient.
set(public_header_dir ${PROJECT_SOURCE_DIR}/include)
file(
    GLOB_RECURSE public_headers
    CONFIGURE_DEPENDS
    RELATIVE ${public_header_dir}
    ${public_header_dir}/xstd/*.hpp
)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/header_self_sufficiency)

foreach(h ${public_headers})
    string(REPLACE "/" "." header_test_id ${h})
    string(REGEX REPLACE "[.]hpp$" "" header_test_id ${header_test_id})
    set(header_test_source ${CMAKE_CURRENT_BINARY_DIR}/header_self_sufficiency/${header_test_id}.cpp)
    file(
        WRITE ${header_test_source}
        "#include <${h}>\nint main() {}\n"
    )

    add_executable(header.${header_test_id} ${header_test_source})
    target_link_libraries(
        header.${header_test_id} PRIVATE
        ${PROJECT_NAME}::${PROJECT_NAME}
        xstd_test_options
    )
    add_test(NAME header.${header_test_id} COMMAND header.${header_test_id})
endforeach()

if(NOT DEFINED Boost_USE_STATIC_LIBS)
    set(Boost_USE_STATIC_LIBS ON)
endif()

find_package(
    Boost CONFIG REQUIRED COMPONENTS
    unit_test_framework
)

set(current_source_dir ${CMAKE_CURRENT_SOURCE_DIR}/src)
file(
    GLOB_RECURSE targets
    CONFIGURE_DEPENDS
    RELATIVE ${current_source_dir}
    ${current_source_dir}/*.cpp
)

foreach(t ${targets})
    cmake_path(GET t PARENT_PATH target_path)
    cmake_path(GET t STEM target_name_we)
    string(REPLACE "/" "." target_id ${target_path}/${target_name_we})
    string(REGEX REPLACE "^[.]" "" target_id ${target_id})

    add_executable(${target_id} src/${t})

    target_link_libraries(
        ${target_id} PRIVATE
        ${PROJECT_NAME}::${PROJECT_NAME}
        Boost::unit_test_framework
        xstd_test_options
    )

    add_test(NAME ${target_id} COMMAND ${target_id} --log_level=all)
endforeach()
