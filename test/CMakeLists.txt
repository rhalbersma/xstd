#          Copyright Rein Halbersma 2014-2026.
# Distributed under the Boost Software License, Version 1.0.
#    (See accompanying file LICENSE_1_0.txt or copy at
#          http://www.boost.org/LICENSE_1_0.txt)

set(Boost_USE_STATIC_LIBS ON)

find_package(
    Boost CONFIG REQUIRED COMPONENTS
    unit_test_framework
)

add_library(xstd_test_options INTERFACE)
option(XSTD_WARNINGS_AS_ERRORS "Treat warnings as errors in xstd tests" ${PROJECT_IS_TOP_LEVEL})

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
    $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>:
        -pedantic-errors
    >
)

set(cxx_compile_options_mbig_obj
    $<$<AND:$<CXX_COMPILER_ID:GNU>,$<PLATFORM_ID:Windows>>:
	    -Wa,-mbig-obj
    >
)

target_compile_definitions(
    xstd_test_options INTERFACE
    ${cxx_compile_definitions}
)
target_compile_options(
    xstd_test_options INTERFACE
    ${cxx_compile_options_warnings}
    ${cxx_compile_options_mbig_obj}
    $<$<AND:$<BOOL:${XSTD_WARNINGS_AS_ERRORS}>,$<CXX_COMPILER_ID:MSVC>>:/WX>
    $<$<AND:$<BOOL:${XSTD_WARNINGS_AS_ERRORS}>,$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>>:-Werror>
)

set(current_source_dir ${CMAKE_CURRENT_SOURCE_DIR}/src)
file(
    GLOB_RECURSE targets
    CONFIGURE_DEPENDS
    RELATIVE ${current_source_dir}
    ${current_source_dir}/*.cpp
)

foreach(t ${targets})
    get_filename_component(target_path ${t} PATH)
    get_filename_component(target_name_we ${t} NAME_WE)
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
