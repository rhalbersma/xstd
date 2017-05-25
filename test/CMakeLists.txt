###############################################################
# use the Boost.Test unit test framework
###############################################################

find_package(Boost 1.64.0 REQUIRED COMPONENTS 
        unit_test_framework
)

list(APPEND cxx_compile_definitions_boost_test 
        BOOST_TEST_DYN_LINK 
        BOOST_TEST_MAIN
)

list(APPEND cxx_compile_options_warnings_boost_test 
        $<$<CXX_COMPILER_ID:Clang>:
                -Wno-disabled-macro-expansion 
                -Wno-global-constructors 
        >
)

###############################################################
# common compiler and linker flags
###############################################################

list(APPEND cxx_compile_options_warnings_general
        $<$<CXX_COMPILER_ID:Clang>:
                -Weverything 
                -Wno-c++98-compat 
                -Wno-c++98-compat-pedantic      
                -Wno-error=padded
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
        -Werror 
        -pedantic-errors  
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

list(APPEND tests 
        cstddef 
        cstdlib 
        int_set
)

set(test_include_dir ${CMAKE_CURRENT_SOURCE_DIR}/include)

foreach(t ${tests})
        add_executable(test.${t} src/${t}.cpp)

        target_include_directories(test.${t} PRIVATE
                ${test_include_dir}
        )

        target_link_libraries(test.${t} PRIVATE
                ${t} 
                ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
                ${cxx_linker_optimization_flags}
        )

        target_compile_definitions(test.${t} PRIVATE
                ${cxx_compile_definitions_boost_test}
        )

        target_compile_options(test.${t} PRIVATE 
                ${cxx_compile_options_warnings_general}
                ${cxx_compile_options_warnings_boost_test}
                ${cxx_compile_options_optimization}     
        )

        add_test(test.${t} test.${t})
endforeach()
