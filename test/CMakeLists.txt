cmake_minimum_required(VERSION 2.8.12...3.17)

project(
		galMemoryTest
		LANGUAGES CXX
)

# --- Import tools ----
include(../cmake/tools.cmake)
# ---- Dependencies ----
include(../cmake/CPM.cmake)
CPMAddPackage(
		NAME cxxopts
		GITHUB_REPOSITORY jarro2783/cxxopts
		VERSION 2.2.0
		OPTIONS "CXXOPTS_BUILD_EXAMPLES Off" "CXXOPTS_BUILD_TESTS Off"
)
CPMAddPackage(NAME galMemory SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/..)

set(BUILD_GMOCK OFF)
set(INSTALL_GTEST OFF)
include(cmake/google-test.cmake)

file(GLOB_RECURSE google-test-source CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/google-test/*.cpp")
file(GLOB_RECURSE test-source CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/test/*.cpp")

add_executable(
		${PROJECT_NAME}
		${google-test-source}
		${test-source}
		main.cpp
)

set_target_properties(
		${PROJECT_NAME}
		PROPERTIES CXX_STANDARD 17
		OUTPUT_NAME "galMemoryTest"
)

target_link_libraries(
		${PROJECT_NAME}
		PRIVATE
		galMemory::galMemory
		gtest_main
		cxxopts
)

add_test(
		NAME ${PROJECT_NAME}_test
		COMMAND ${PROJECT_NAME}
)
