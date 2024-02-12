# base definitions
cmake_minimum_required(VERSION 3.20 FATAL_ERROR)
set(PROJECT_NAME topic_evolution_model)
project(${PROJECT_NAME})
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pipe -O3")

# dependencies
include(FetchContent)
FetchContent_Declare(cpr GIT_REPOSITORY https://github.com/libcpr/cpr.git
        GIT_TAG 3b15fa82ea74739b574d705fea44959b58142eb8) # Replace with your desired git commit from: https://github.com/libcpr/cpr/releases
FetchContent_MakeAvailable(cpr)

# executable definition
AUX_SOURCE_DIRECTORY(./src SRC)
message(STATUS "SOURCE FILES: ${SRC}")
add_executable(${PROJECT_NAME} ${SRC})
target_link_libraries(${PROJECT_NAME} PRIVATE cpr::cpr)

# tests
enable_testing()
AUX_SOURCE_DIRECTORY(./src/test SRC_TEST)
add_executable(${PROJECT_NAME}_test ${SRC} ${SRC_TEST})
target_link_libraries(${PROJECT_NAME}_test PRIVATE cpr::cpr)
target_compile_definitions(${PROJECT_NAME}_test PRIVATE TESTING_ENV)
add_test(NAME Tests COMMAND ${PROJECT_NAME}_test)

