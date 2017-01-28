###############################################################
# use the Boost.Test unit testing framework
###############################################################

find_package(Boost 1.60.0 REQUIRED COMPONENTS unit_test_framework)

###############################################################
# common compiler and linker flags
###############################################################

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    list(APPEND cxx_compile_warning_flags
        -Weverything 
        -Werror 
        -pedantic-errors
        -Wno-c++98-compat 
        -Wno-c++98-compat-pedantic      
    )  
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    list(APPEND cxx_compile_warning_flags
        -Wall
        -Wextra
        -Wpedantic
        -Werror 
        -pedantic-errors  
    )  
endif()

list(APPEND cxx_compile_disabled_warning_flags_boost_test 
    -Wno-disabled-macro-expansion 
    -Wno-global-constructors 
)

list(APPEND cxx_compile_optimization_flags 
    -g 
    -O3
    -flto 
    -march=native 
    -mtune=native
)

list(APPEND cxx_linker_optimization_flag
    -flto 
    -fuse-ld=gold
)

###############################################################
# define test executables
###############################################################

add_executable(test.cstddef src/cstddef.cpp)

target_link_libraries(test.cstddef PRIVATE
    cstddef 
    ${Boost_LIBRARIES}
    ${cxx_linker_optimization_flag}    
)

target_compile_definitions(test.cstddef PRIVATE 
    BOOST_TEST_DYN_LINK 
    BOOST_TEST_MAIN
)

target_compile_options(test.cstddef PRIVATE 
    ${cxx_compile_warning_flags}
    ${cxx_compile_disabled_warning_flags_boost_test}
    ${cxx_compile_optimization_flags}     
)

add_test(test.cstddef test.cstddef)

###############################################################
 
add_executable(test.cstdlib src/cstdlib.cpp)

target_link_libraries(test.cstdlib PRIVATE
    cstdlib 
    ${Boost_LIBRARIES}
    ${cxx_linker_optimization_flag}    
)

target_compile_definitions(test.cstdlib PRIVATE 
    BOOST_TEST_DYN_LINK 
    BOOST_TEST_MAIN
)

target_compile_options(test.cstdlib PRIVATE 
    ${cxx_compile_warning_flags}
    ${cxx_compile_disabled_warning_flags_boost_test}
    ${cxx_compile_optimization_flags}     
    -Wno-exit-time-destructors
)

add_test(test.cstdlib test.cstdlib)

###############################################################
 
add_executable(test.int_set src/int_set.cpp)

target_include_directories(test.int_set PRIVATE
    include
)

target_link_libraries(test.int_set PRIVATE
    int_set 
    ${Boost_LIBRARIES}
    ${cxx_linker_optimization_flag}    
)

target_compile_definitions(test.int_set PRIVATE 
    BOOST_TEST_DYN_LINK 
    BOOST_TEST_MAIN
)

target_compile_options(test.int_set PRIVATE 
    ${cxx_compile_warning_flags}
    ${cxx_compile_disabled_warning_flags_boost_test}
    ${cxx_compile_optimization_flags}     
    -Wno-padded
)

add_test(test.int_set test.int_set)
 