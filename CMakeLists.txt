cmake_minimum_required(VERSION 3.10)
project(nixie)

set (CMAKE_CXX_STANDARD 20)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set (DPP_BUILD_TEST OFF CACHE STRING "" FORCE)
set (DPP_INSTALL OFF CACHE STRING "" FORCE)
set (DPP_USE_EXTERNAL_JSON ON CACHE STRING "" FORCE)

include(FetchContent)

FetchContent_Declare(dpp
    GIT_REPOSITORY https://github.com/brainboxdotcc/DPP
    GIT_TAG v10.1.2 
)

FetchContent_Declare(spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog
    GIT_TAG v1.15.1
)
FetchContent_Declare(nlohmann
    GIT_REPOSITORY https://github.com/nlohmann/json
    GIT_TAG v3.11.3
)
FetchContent_Declare(utf8
    GIT_REPOSITORY https://github.com/nemtrif/utfcpp
    GIT_TAG 65701fe00700577f9f4e9b96d682bd7cfcc0b76e
)

FetchContent_MakeAvailable(
    spdlog
    nlohmann
    dpp
    utf8
)

if (LINUX)
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
    set (CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
endif()

add_subdirectory(src)
add_subdirectory(tests EXCLUDE_FROM_ALL)

add_custom_target(run
    COMMAND nixie
    DEPENDS nixie
    COMMENT "Run nixie")
add_custom_target(test
    COMMAND tests
    DEPENDS tests
    COMMENT "Test nixie")
# vim:ft=cmake
