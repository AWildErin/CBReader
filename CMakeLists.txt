cmake_minimum_required(VERSION 3.16 FATAL_ERROR)
project(CBReader)
set(CMAKE_CXX_STANDARD 20)

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/bufferstream")

option(BUILD_TESTS "Build tests for the CB Reader" ON)

set(CBREADER_SRC
	${CMAKE_CURRENT_LIST_DIR}/include/cbreader/types.h

	${CMAKE_CURRENT_LIST_DIR}/include/cbreader/rmesh.h
	${CMAKE_CURRENT_LIST_DIR}/src/rmesh.cpp
)

add_library(${PROJECT_NAME} STATIC ${CBREADER_SRC})
target_link_libraries(${PROJECT_NAME} PUBLIC bufferstream)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)
#target_precompile_headers(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/include/cbreader/types.h)

# Used with permission from:
# https://github.com/craftablescience/studiomodelpp/blob/main/CMakeLists.txt#L32
if(BUILD_TESTS)
    include(FetchContent)
    FetchContent_Declare(
            googletest
            GIT_REPOSITORY https://github.com/google/googletest.git
            GIT_TAG v1.14.0)
    FetchContent_MakeAvailable(googletest)
    enable_testing()

    set(TEST_RESOURCES_ROOT_PATH "${CMAKE_CURRENT_LIST_DIR}/test/res/")
    configure_file("${CMAKE_CURRENT_LIST_DIR}/test/config.h.in" "${CMAKE_CURRENT_LIST_DIR}/test/config.h")

    add_executable(${PROJECT_NAME}Test 
        "${CMAKE_CURRENT_LIST_DIR}/test/test_rmesh.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/test/main.cpp"
    )
    target_link_libraries(${PROJECT_NAME}Test PUBLIC ${PROJECT_NAME} gtest_main)

	#include(GoogleTest)
	#gtest_discover_tests(${PROJECT_NAME}Test)
endif()