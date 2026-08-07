#          Copyright Rein Halbersma 2014-2026.
# Distributed under the Boost Software License, Version 1.0.
#    (See accompanying file LICENSE_1_0.txt or copy at
#          http://www.boost.org/LICENSE_1_0.txt)

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
        # The same category, one standard on. "Would not have compiled before
        # C++20" says nothing to a library that requires C++23, and the pair
        # above has already made that judgement: it is what keeps <=> and
        # defaulted comparisons quiet, which reach Clang as -Wpre-c++20-compat.
        # -Wc++20-compat is a separate group those two do not cover, and it
        # fires on exactly one construct here - the implicit typename in
        # make_unsigned_like_t, which is the spelling clang-tidy's
        # readability-redundant-typename requires. Without this the two tools
        # deadlock over a single token.
        -Wno-c++20-compat
        -Wno-disabled-macro-expansion       # triggered by Boost.Test
        -Wno-global-constructors            # triggered by Boost.Test
        -Wno-used-but-marked-unused         # triggered by Boost.Test
        -Wno-c2y-extensions                 # triggered by Boost.Test's use of __COUNTER__
        -Wno-lifetime-safety-lifetimebound-violation  # can't verify chained operator<< returns ostr
        # xstd::to_chars takes and returns raw pointers because std::to_chars
        # does: [charconv.to.chars] specifies the interface as a char* range,
        # and matching it is the point. Every pointer step inside that contract
        # draws this warning, as it would for the standard library's own
        # implementation, so it says nothing specific to this code.
        -Wno-unsafe-buffer-usage
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
        # A false positive inside libstdc++ itself, not in anything this
        # project writes. Formatting a div_t through the tuple formatter
        # instantiates std::__unicode::__field_width for wchar_t, and at -O3
        # GCC 15's inliner reports _Grapheme_cluster_iterator_base::_M_c as
        # maybe-uninitialized in <bits/unicode.h>. System-header suppression
        # does not reach it because the diagnostic is raised after inlining.
        # Scoped no further than the compiler that raises it: the finding is
        # in a standard-library header, so no spelling on this side removes
        # it, and the alternative is not formatting a div_t as a wide string.
        -Wno-maybe-uninitialized
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
# Configuration headers are implementation details included by public headers.
list(FILTER public_headers EXCLUDE REGEX "^xstd/config/")
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/header_self_sufficiency)

foreach(h ${public_headers})
    string(REPLACE "/" "." header_test_id ${h})
    string(REGEX REPLACE "[.]hpp$" "" header_test_id ${header_test_id})
    set(header_test_source ${CMAKE_CURRENT_BINARY_DIR}/header_self_sufficiency/${header_test_id}.cpp)
    # main is spelled with a trailing return type because these generated
    # sources are also what clang-tidy runs over, and modernize-use-trailing-
    # return-type (enabled, like the rest of modernize-*) flags "int main()".
    file(
        WRITE ${header_test_source}
        "#include <${h}> // IWYU pragma: keep\nauto main() -> int {}\n"
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
    Boost 1.70 CONFIG REQUIRED COMPONENTS
    unit_test_framework
)

# Boost.Int128 is the one 128-bit type in the test suite that xstd knows
# nothing about, which is what makes it worth testing against: see
# test/include/xstd/test/boost_int128.hpp. It is not a hard dependency - the
# tests fall back to the types xstd provides itself when it is absent.
#
# It ships with Boost 1.92, whose per-library package config this call finds
# alongside a standalone or vcpkg install. Until that release, fetch the
# upstream repository. Once 1.92 is the floor, the fetch below can go and this
# can become a component of the find_package above.
option(XSTD_TEST_FETCH_BOOST_INT128 "Fetch Boost.Int128 when no installed copy is found" ON)

find_package(boost_int128 CONFIG QUIET)
if(NOT TARGET Boost::int128 AND XSTD_TEST_FETCH_BOOST_INT128)
    include(FetchContent)
    FetchContent_Declare(
        boost_int128
        GIT_REPOSITORY https://github.com/cppalliance/int128.git
        # A development commit rather than the latest release tag, which is
        # v3.1.0: the types lost their _t suffix after it was cut, and the
        # un-suffixed spelling is the one the documentation for the Boost
        # release carries. Pinned rather than tracking a branch, since this is
        # a library still being prepared for its first Boost release.
        GIT_TAG f437657470186b79f1f314bc2c76a112bcc2529c
        # Its own CMakeLists.txt builds its test suite whenever BUILD_TESTING
        # is on, which is exactly when this project configures it. Naming a
        # subdirectory that holds no CMakeLists.txt keeps the download and
        # leaves its build system out of this one.
        SOURCE_SUBDIR does-not-exist
    )
    FetchContent_MakeAvailable(boost_int128)

    # SYSTEM because these headers are not written to the warning level this
    # project holds its own code to: at -Weverything they report -Wpadded and
    # -Wextra-semi-stmt, neither of which says anything about xstd. An
    # installed copy arrives through an imported target, which CMake already
    # treats as system.
    add_library(Boost::int128 INTERFACE IMPORTED)
    target_include_directories(Boost::int128 SYSTEM INTERFACE ${boost_int128_SOURCE_DIR}/include)
endif()

if(TARGET Boost::int128)
    set(xstd_test_boost_int128 Boost::int128)
else()
    set(xstd_test_boost_int128)
endif()

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
        ${xstd_test_boost_int128}
        xstd_test_options
    )

    add_test(NAME ${target_id} COMMAND ${target_id} --log_level=all)
endforeach()
