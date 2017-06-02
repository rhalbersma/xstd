###############################################################
# project settings
###############################################################

cmake_minimum_required(VERSION 3.4)
project(xstd CXX)

set(project_include_dir
	$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
	$<INSTALL_INTERFACE:$<INSTALL_PREFIX>/include>
)

###############################################################

add_library(cstddef     INTERFACE)
add_library(cstdint     INTERFACE)
add_library(cstdlib     INTERFACE)
add_library(int_set     INTERFACE)
add_library(type_traits INTERFACE)

###############################################################

target_include_directories(cstddef     INTERFACE ${project_include_dir})
target_include_directories(cstdint     INTERFACE ${project_include_dir})
target_include_directories(cstdlib     INTERFACE ${project_include_dir})
target_include_directories(int_set     INTERFACE ${project_include_dir})
target_include_directories(type_traits INTERFACE ${project_include_dir})

###############################################################

target_compile_features(cstddef     INTERFACE cxx_constexpr cxx_inline_namespaces cxx_user_literals)
target_compile_features(cstdint     INTERFACE cxx_alias_templates)
target_compile_features(cstdlib     INTERFACE cxx_noexcept cxx_relaxed_constexpr)
target_compile_features(int_set     INTERFACE cxx_std_17)
target_compile_features(type_traits INTERFACE cxx_std_17)

###############################################################

#TODO: install()

###############################################################
# enable unit testing
###############################################################

include(CTest)
add_subdirectory(test)
