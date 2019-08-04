cmake_minimum_required(VERSION 3.15)
if(POLICY CMP0048)
  cmake_policy(SET CMP0048 NEW)
endif()
project(acatl VERSION 0.2.0 LANGUAGES CXX)

include(CheckFunctionExists)
include(FetchContent)

option(BUILD_TESTS "Build test programs" OFF)
option(BUILD_EXAMPLES "Build example programs" OFF)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/archive)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(ACATL_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(ACATL_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(ACATL_VERSION_PATCH ${PROJECT_VERSION_PATCH})

set(ACATL_VERSION ${ACATL_VERSION_MAJOR}.${ACATL_VERSION_MINOR}.${ACATL_VERSION_PATCH})
set(ACATL_VERSION_SHORT ${ACATL_VERSION_MAJOR}.${ACATL_VERSION_MINOR})
set(ACATL_VERSION_STRING ${ACATL_VERSION_MAJOR}.${ACATL_VERSION_MINOR}.${ACATL_VERSION_PATCH})
set(ACATL_VERSION_HEX 0x00000200)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

enable_testing()

if(CMAKE_BUILD_TYPE MATCHES "Debug")
    message("Setting up ACATL ${ACATL_VERSION_STRING} for a debug build")
elseif(CMAKE_BUILD_TYPE MATCHES "RelWithDebInfo")
    message("Setting up ACATL ${ACATL_VERSION_STRING} for a release with debug info build")
elseif(CMAKE_BUILD_TYPE MATCHES "Release")
    message("Setting up ACATL ${ACATL_VERSION_STRING} for a release build")
else()
    message(FATAL_ERROR "Unknown build type ${CMAKE_BUILD_TYPE} specified")
endif()

include_directories(${CMAKE_BINARY_DIR})
include_directories(${CMAKE_SOURCE_DIR})

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

include(cmake/fetch_asio.cmake)
include(cmake/fetch_date.cmake)
include(cmake/fetch_nlohmann.cmake)
include(cmake/fetch_linenoise.cmake)
include(cmake/fetch_argtable.cmake)
include(cmake/fetch_http_parser.cmake)

find_package(OpenSSL REQUIRED)

if(NOT WIN32)
    CHECK_FUNCTION_EXISTS(uuid_generate HAS_UUID)
    if(NOT HAS_UUID)
        message("-- Looking for libuuid")
        find_path(UUID_INCLUDE_DIR uuid/uuid.h)
        find_library(UUID_LIBRARY NAMES uuid)
        if(UUID_INCLUDE_DIR AND UUID_LIBRARY)
            message("-- Found libuuid: ${UUID_LIBRARY}")
            include_directories(${UUID_INCLUDE_DIR})
            set(ACATL_PLATFORM_LIBS ${UUID_LIBRARY})
        endif()
        set(HAS_UUID 1)
    endif()
else()
    message(STATUS "Currently not supported")
endif()

if(NOT OpenSSL_FOUND)
    message(FATAL_ERROR "Could not find OpenSSL")
endif()

if(NOT HAS_UUID)
    message(FATAL_ERROR "Could not find libuuid")
endif()

if(NOT WIN32)
    include_directories(SYSTEM ${OPENSSL_INCLUDE_DIR})
    link_directories(${OPENSSL_LIBRARIES} /usr/local/lib)
else()
    include_directories(SYSTEM ${OPENSSL_INCLUDE_DIR})
    link_directories(${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
endif()

if(CMAKE_BUILD_TYPE MATCHES "Debug")
    add_definitions(-DDEBUG)
endif()

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT APPLE AND UNIX)
    set(ACATL_PLATFORM_LIBS ${ACATL_PLATFORM_LIBS} dl pthread)
    if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -Wheader-hygiene -Wcast-align -Wconversion -Wfloat-equal -Wformat=2 -Wimplicit-atomic-properties -Wmissing-declarations -Woverlength-strings -Wshadow -Wstrict-selector-match -Wundeclared-selector -Wunreachable-code -Wextra -Wno-unused-parameter -Wpedantic -Wnon-virtual-dtor -Werror")
    elseif()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -Werror")
    endif()
    message(STATUS "${SANITIZE}")
    if(SANITIZE MATCHES "On")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -O1 -fno-omit-frame-pointer")
    endif()
    add_definitions(-DHAS_PUT_TIME -DHAS_GET_TIME)
elseif(APPLE AND CMAKE_GENERATOR STREQUAL Xcode)
    message("Setting up Xcode for C++14 with libc++.")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wheader-hygiene -Wcast-align -Wconversion -Wfloat-equal -Wformat=2
                         -Wimplicit-atomic-properties -Wmissing-declarations -Wmissing-prototypes -Woverlength-strings
                         -Wshadow -Wstrict-selector-match -Wundeclared-selector -Wunreachable-code -Wextra
                         -Wno-unused-parameter -Wpedantic -Wnon-virtual-dtor -Werror")
elseif(APPLE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wheader-hygiene -Wcast-align -Wconversion -Wfloat-equal -Wformat=2
                         -Wimplicit-atomic-properties -Wmissing-declarations -Wmissing-prototypes -Woverlength-strings
                         -Wshadow -Wstrict-selector-match -Wundeclared-selector -Wunreachable-code -Wextra
                         -Wno-unused-parameter -Wpedantic -Wnon-virtual-dtor -Werror")
elseif(WIN32)
    add_definitions(-DNOMINMAX)
    set(CMAKE_CXX_FLAGS "-D_SCL_SECURE_NO_WARNINGS")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /MP")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP /EHsc")
    set(ACATL_PLATFORM_LIBS )
else()
    message(FATAL_ERROR "unknown platform")
endif()

add_subdirectory(acatl)
add_subdirectory(acatl_application)
add_subdirectory(acatl_mqtt)
add_subdirectory(acatl_network)

if(BUILD_EXAMPLES)
  add_subdirectory(examples)
endif(BUILD_EXAMPLES)

if(BUILD_TESTS)
  set(GTEST_LIBRARIES gtest gmock)
  add_subdirectory(test)

  FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        release-1.8.0
  )

  if(NOT googletest_POPULATED)
    FetchContent_Populate(googletest)
    # GTest framework has to be linked into the acatl root direcrtory
    # Disable all warings that might be generate from building the gtest framework
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -w")
    add_subdirectory(${googletest_SOURCE_DIR})
  endif()
endif(BUILD_TESTS)

