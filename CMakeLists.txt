cmake_minimum_required(VERSION 3.27)

project(cztodoby)

add_library(cztodoby INTERFACE
	"./source/crazygaze/TodoBy.h"
)

# Setup a custom target that doesn't do anything and is just used to show the headers in Visual Studio
# add_custom_target(cztodoby_ SOURCES "./source/crazygaze/TodoBy.h")

target_include_directories(cztodoby INTERFACE "./source")
target_compile_features(cztodoby INTERFACE cxx_std_20)

if (PROJECT_IS_TOP_LEVEL)
	add_executable(cztodoby_tests "./tests.cpp")
	target_link_libraries(cztodoby_tests cztodoby)
	if (MSVC)
		target_compile_options(cztodoby_tests PRIVATE /Wall)
	endif()
endif()

