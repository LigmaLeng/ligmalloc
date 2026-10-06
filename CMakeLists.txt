cmake_minimum_required(VERSION 4.2)

project(ligmarena LANGUAGES C)
set(CMAKE_C_COMPILER gcc)

if(NOT CMAKE_BUILD_TYPE)
    set(default_build_type "Debug")
    message(STATUS
        "Set the build type to `${default_build_type}` as none was specified."
    )
    set(CMAKE_BUILD_TYPE ${default_build_type} CACHE STRING
        "Chooce the build type." FORCE
    )
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
        "Debug" "Release" "MinSizeRel" "RelWithDebInfo"
    )
endif()
message(STATUS "ligmarena Build Type: ${CMAKE_BUILD_TYPE}")

# Set the version for ligmarena
set(Ligmarena_Version_Major 0)
set(Ligmarena_Version_Minor 1)
set(Ligmarena_Version_Patch 0)
set(Ligmarena_Version_Status "-dev")
set(PROJECT_VERSION
"${Ligmarena_Version_Major}.${Ligmarena_Version_Minor}.\
${Ligmarena_Version_Patch}${Ligmarena_Version_Status}\
"
)
message(STATUS "${PROJECT_NAME} version: ${PROJECT_VERSION}")

set(CMAKE_C_FLAGS "-march=native")
set(CMAKE_C_FLAGS_DEBUG "-g")
set(CMAKE_C_FLAGS_RELEASE "-O3 -DNDEBUG")
set(CMAKE_C_FLAGS_RELWITHDEBINFO "-O2 -g -DNDEBUG")
set(CMAKE_C_FLAGS_MINSIZEREL "-Os -DNDEBUG")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
add_library(safe_macros STATIC ${CMAKE_CURRENT_SOURCE_DIR}/src/safe_macros.c)
add_library(ligmarena STATIC ${CMAKE_CURRENT_SOURCE_DIR}/src/ligmarena.c)
