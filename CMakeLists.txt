###############################################################
# project settings
###############################################################

cmake_minimum_required(VERSION 3.11)
project(xstd CXX)

set(project_include_dir
	$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
	$<INSTALL_INTERFACE:$<INSTALL_PREFIX>/include>
)

###############################################################

add_library(xstd INTERFACE)
target_include_directories(xstd INTERFACE ${project_include_dir})
target_compile_features(xstd INTERFACE cxx_std_17)

###############################################################

#TODO: install()

###############################################################
# enable unit testing
###############################################################

include(CTest)
add_subdirectory(test)
