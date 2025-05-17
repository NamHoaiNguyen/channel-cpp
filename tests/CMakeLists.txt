cmake_minimum_required(VERSION 3.24)
# project(ChannelCpp LANGUAGES CXX)

include(FetchContent)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/release-1.12.1.zip
)

# To avoid overriding compiler/linker settings on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

add_executable(channel_test channel_test.cpp)

target_link_libraries(channel_test PRIVATE ChannelCpp gtest_main)

include(GoogleTest)
gtest_discover_tests(channel_test)
