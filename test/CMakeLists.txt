###############################################################
# use the Boost.Test unit test framework
###############################################################

set(BOOST_INCLUDEDIR /usr/local/include/boost-1_65_1)
set(BOOST_LIBRARYDIR /usr/local/lib)
set(Boost_COMPILER -gcc)
#set(Boost_COMPILER -clang50) 

find_package(Boost 1.65.1 REQUIRED COMPONENTS 
        unit_test_framework
)

set(cxx_compile_definitions_boost_test 
        BOOST_TEST_DYN_LINK 
        BOOST_TEST_MAIN
)

set(cxx_compile_options_warnings_boost_test 
        $<$<CXX_COMPILER_ID:Clang>:
                -Wno-disabled-macro-expansion 
                -Wno-global-constructors 
        >
)

###############################################################
# common compiler and linker flags
###############################################################

set(cxx_compile_options_warnings_general
        $<$<CXX_COMPILER_ID:Clang>:
                -Weverything 
                -Wno-c++98-compat 
                -Wno-c++98-compat-pedantic      
                -Wno-range-loop-analysis
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
        -Wno-padded
        -Wno-deprecated-declarations
)  

set(cxx_compile_options_optimization 
#	--coverage
#       -stdlib=libc++
        -g
        -O3
        -flto 
        -march=native 
        -mtune=native
)

set(cxx_linker_optimization_flags
#		-stdlib=libc++
#		-lc++abi
        -flto 
        -fuse-ld=gold
)

###############################################################
# define test executables
###############################################################

set(test_include_dir ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(test_source_dir ${CMAKE_CURRENT_SOURCE_DIR}/src)
file(GLOB_RECURSE targets RELATIVE ${test_source_dir} *.cpp)

foreach(t ${targets})
		get_filename_component(target_path ${t} PATH)
		get_filename_component(target_name_we ${t} NAME_WE)
		string(REPLACE "/" "." target_id ${target_path}/${target_name_we})
		string(REGEX REPLACE "^[.]" "" target_id ${target_id})

		add_executable(${target_id} src/${t})
	
        target_include_directories(${target_id} PRIVATE 
        		${test_include_dir}
        )
 
        target_include_directories(${target_id} SYSTEM PRIVATE 
        		${Boost_INCLUDE_DIR}
        )

        target_link_libraries(${target_id} PRIVATE
                xstd 
                ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
                ${cxx_linker_optimization_flags}
        )

        target_compile_definitions(${target_id} PRIVATE 
                ${cxx_compile_definitions_boost_test}
        )

        target_compile_options(${target_id} PRIVATE 
                ${cxx_compile_options_warnings_general}
                ${cxx_compile_options_warnings_boost_test}
                ${cxx_compile_options_optimization}     
        )

        add_test(${target_id} ${target_id})  
endforeach()
