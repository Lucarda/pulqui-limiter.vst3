
cmake_minimum_required (VERSION 3.25.0)

option(GITHUB-CI "If we are running the github builds" OFF)

set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64")
set(CMAKE_XCODE_ATTRIBUTE_CODE_SIGNING_ALLOWED "NO")

#-------------------------------------------------------------------------------
# Remove macOS code signing in sources
#-------------------------------------------------------------------------------

# Define the source and destination paths
set(SOURCE_FILE "${CMAKE_SOURCE_DIR}/xcodehack.txt")
set(DESTINATION_DIR "${CMAKE_SOURCE_DIR}/cmake/modules")

# Copy the file
file(COPY "${SOURCE_FILE}" DESTINATION "${DESTINATION_DIR}")

# Rename old_file.txt to new_file.txt
file(RENAME "${CMAKE_SOURCE_DIR}/cmake/modules/xcodehack.txt" "${CMAKE_SOURCE_DIR}/cmake/modules/SMTG_AddVST3Library.cmake")
#-------------------------------------------------------------------------------



# Specify the minimum version of the target platform
if(NOT DEFINED ENV{MACOSX_DEPLOYMENT_TARGET})
    set(CMAKE_OSX_DEPLOYMENT_TARGET "10.13" CACHE STRING "macOS deployment target")
endif()

# Global options which can bet set on command line e.g.: cmake -DSMTG_ENABLE_VST3_PLUGIN_EXAMPLES=OFF ...
option(SMTG_ENABLE_VST3_PLUGIN_EXAMPLES "Enable VST 3 Plug-in Examples" OFF)
option(SMTG_ENABLE_VST3_HOSTING_EXAMPLES "Enable VST 3 Hosting Examples" OFF)
option(SMTG_ENABLE_VSTGUI_SUPPORT "Enable VSTGUI Support" ON)

#-------------------------------------------------------------------------------
# Includes
#-------------------------------------------------------------------------------

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules")

include(SMTG_VST3_SDK)



#-------------------------------------------------------------------------------
# SDK Project
#-------------------------------------------------------------------------------
project(vstsdk
    VERSION 3.7.14
    DESCRIPTION "Steinberg VST 3 Software Development Kit"
    HOMEPAGE_URL "https://www.steinberg.net"
)

smtg_setup_platform_toolset()
smtg_setup_symbol_visibility()

set(ROOT "${CMAKE_CURRENT_SOURCE_DIR}")

# Set the location of the VST 3 SDK
set(SDK_ROOT "${ROOT}")
set(public_sdk_SOURCE_DIR ${SDK_ROOT}/public.sdk)
set(pluginterfaces_SOURCE_DIR ${SDK_ROOT}/pluginterfaces)

if(SMTG_ENABLE_VSTGUI_SUPPORT)
    smtg_enable_vstgui_support(VSTGUI_SOURCE_DIR "${ROOT}/vstgui4")
endif()

include_directories(${ROOT} ${SDK_ROOT})

#-------------------------------------------------------------------------------
# From Here this is optional...

#-------------------------------------------------------------------------------
# Projects
#-------------------------------------------------------------------------------
set(SDK_IDE_LIBS_FOLDER FOLDER "Libraries")

#---Add base libraries---------------------------
set(VST_SDK TRUE) # used for pluginterfaces and public.sdk modules which provides only a subset of them for VST-SDK
add_subdirectory(pluginterfaces)
add_subdirectory(base)
add_subdirectory(public.sdk)
add_subdirectory(public.sdk/source/vst/interappaudio)


# Add hosting examples, it includes the validator (must be done before any plug-ins to support running the validator after building)
set(SDK_IDE_HOSTING_EXAMPLES_FOLDER FOLDER "Hosting-Examples")
add_subdirectory(public.sdk/samples/vst-hosting)

# Add utilities
set(SDK_IDE_UTILITIES_FOLDER FOLDER "Utilities")
add_subdirectory(public.sdk/samples/vst-utilities)


#-------------------------------------------------------------------------------
# build plugins
#-------------------------------------------------------------------------------


# limiter
add_subdirectory(pulquilimiter-src)

# crossover 4bands
add_subdirectory(pulquicrossover4-src)

# crossover 3bands
add_subdirectory(pulquicrossover3-src)

# crossover 2bands
add_subdirectory(pulquicrossover2-src)
