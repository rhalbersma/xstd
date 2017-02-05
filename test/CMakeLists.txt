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
    list(APPEND cxx_compile_options_warnings
        -Weverything 
        -Werror 
        -pedantic-errors
        -Wno-c++98-compat 
        -Wno-c++98-compat-pedantic      
        -Wno-padded
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
    #--coverage      
    -flto 
    -march=native 
    -mtune=native
)

list(APPEND cxx_linker_optimization_flags
    #--coverage
    -flto 
    -fuse-ld=gold
)

###############################################################
# define test executables
###############################################################

list(APPEND tests 
    cstddef 
    cstdlib 
    int_set
)

foreach(t ${tests})
    add_executable(test.${t} src/${t}.cpp)
    
    target_include_directories(test.${t} PRIVATE
        include
    )
    
    target_link_libraries(test.${t} PRIVATE
        ${t} 
        ${Boost_LIBRARIES}
        ${cxx_linker_optimization_flags}    
    )
    
    target_compile_definitions(test.${t} PRIVATE 
        ${cxx_compile_definitions_boost_test}
    )
    
    target_compile_options(test.${t} PRIVATE 
        ${cxx_compile_options_warnings}
        ${cxx_compile_options_boost_test}
        ${cxx_compile_options_optimization}     
    )
    
    add_test(test.${t} test.${t})
endforeach()
