cmake_minimum_required(VERSION 3.27)

project(cztodoby)

add_library(cztodoby INTERFACE
	"./source/crazygaze/TodoBy.h"
)

# Setup a custom target that doesn't do anything and is just used to show the headers in Visual Studio
# add_custom_target(cztodoby_ SOURCES "./source/crazygaze/TodoBy.h")

target_include_directories(cztodoby INTERFACE "./source")
target_compile_features(cztodoby INTERFACE cxx_std_20)

# Set a define with the user name, so we can use them in time bombs that should only trigger for a specific user
# The USERNAME environment variable exists for both Windows and Linux
string(TOLOWER $ENV{USERNAME} username)
message(STATUS "Setting CZTODOBY_USER environment variable to \"${username}\"")
target_compile_definitions(cztodoby INTERFACE CZTODOBY_USER="${username}")

if (PROJECT_IS_TOP_LEVEL)
	add_executable(cztodoby_tests "./tests.cpp")
	target_link_libraries(cztodoby_tests cztodoby)
	if (MSVC)
		target_compile_options(cztodoby_tests PRIVATE /Wall)
	endif()
endif()


