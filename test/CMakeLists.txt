###############################################################
# use the Boost.Test unit testing framework
###############################################################

find_package(Boost 1.60.0 REQUIRED COMPONENTS unit_test_framework)

###############################################################
# common compiler and linker flags
###############################################################

list(APPEND cxx_compile_definitions_boost_test
    BOOST_TEST_DYN_LINK 
    BOOST_TEST_MAIN
)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    list(APPEND cxx_compile_optins_warnings
        -Weverything 
        -Werror 
        -pedantic-errors
        -Wno-c++98-compat 
        -Wno-c++98-compat-pedantic      
    )  
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    list(APPEND cxx_compile_options_warnings
        -Wall
        -Wextra
        -Wpedantic
        -Werror 
        -pedantic-errors  
    )  
endif()

list(APPEND cxx_compile_options_boost_test 
    -Wno-disabled-macro-expansion 
    -Wno-global-constructors 
)

list(APPEND cxx_compile_options_optimization 
    -g 
    -O3
    -flto 
    -march=native 
    -mtune=native
)

list(APPEND cxx_linker_optimization_flags
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
    ${cxx_linker_optimization_flags}    
)

target_compile_definitions(test.cstddef PRIVATE 
    ${cxx_compile_definitions_boost_test}
)

target_compile_options(test.cstddef PRIVATE 
    ${cxx_compile_options_warnings}
    ${cxx_compile_options_boost_test}
    ${cxx_compile_options_optimization}     
)

add_test(test.cstddef test.cstddef)

###############################################################
 
add_executable(test.cstdlib src/cstdlib.cpp)

target_link_libraries(test.cstdlib PRIVATE
    cstdlib 
    ${Boost_LIBRARIES}
    ${cxx_linker_optimization_flags}    
)

target_compile_definitions(test.cstdlib PRIVATE 
    ${cxx_compile_definitions_boost_test}
)

target_compile_options(test.cstdlib PRIVATE 
    ${cxx_compile_options_warnings}
    ${cxx_compile_options_boost_test}
    ${cxx_compile_options_optimization}     
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
    ${cxx_linker_optimization_flags}    
)

target_compile_definitions(test.int_set PRIVATE 
    ${cxx_compile_definitions_boost_test}
)

target_compile_options(test.int_set PRIVATE 
    ${cxx_compile_options_warnings}
    ${cxx_compile_options_boost_test}
    ${cxx_compile_options_optimization}     
)

add_test(test.int_set test.int_set)
 