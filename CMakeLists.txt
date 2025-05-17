cmake_minimum_required(VERSION 3.24)
project(ChannelCpp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(BUILD_TESTS "Build tests" ON)
option(BUILD_EXAMPLES "Build examples" ON)

add_library(ChannelCpp INTERFACE)

# Specify include directories for the interface library
target_include_directories(ChannelCpp INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>  # for install usage
)

option(BUILD_TESTS_DEBUG "Enable Debug for tests" OFF)

if(BUILD_TESTS)
    enable_testing()

    if(BUILD_TESTS_DEBUG)
      set(CMAKE_BUILD_TYPE Debug)
      message(STATUS "Building tests in Debug mode")
    else()
      set(CMAKE_BUILD_TYPE Release)
      message(STATUS "Building tests in Release mode")
  endif()

    add_subdirectory(tests)
endif()

if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

# === Install rules (optional, for reuse) ===
include(GNUInstallDirs)

install(TARGETS ChannelCpp EXPORT ChannelCppTargets)
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(EXPORT ChannelCppTargets
    FILE MChannelCppTargets.cmake
    NAMESPACE ChannelCpp::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ChannelCpp
)