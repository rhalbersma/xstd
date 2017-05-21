###############################################################
# project settings
###############################################################

cmake_minimum_required(VERSION 3.5)
project(xstd CXX)

###############################################################

add_library(cstddef     INTERFACE)
add_library(cstdint     INTERFACE)
add_library(cstdlib     INTERFACE)
add_library(int_set     INTERFACE)
add_library(type_traits INTERFACE)

###############################################################

target_include_directories(cstddef     INTERFACE include)
target_include_directories(cstdint     INTERFACE include)
target_include_directories(cstdlib     INTERFACE include)
target_include_directories(int_set     INTERFACE include)
target_include_directories(type_traits INTERFACE include)

###############################################################

# c++11 compatible
target_compile_features(cstddef INTERFACE
    cxx_constexpr
    cxx_inline_namespaces
    cxx_user_literals
)

# c++11 compatible
target_compile_features(cstdint INTERFACE
    cxx_alias_templates
)

# c++14 compatible
target_compile_features(cstdlib INTERFACE
    cxx_noexcept
    cxx_relaxed_constexpr
)

# c++17 compatible
target_compile_features(int_set INTERFACE
    cxx_std_17
)

# c++14 compatible
target_compile_features(type_traits INTERFACE
    cxx_constexpr
    cxx_noexcept
    cxx_return_type_deduction
)

###############################################################
# enable unit testing
###############################################################

include(CTest)
add_subdirectory(test)
