cmake_minimum_required(VERSION 3.19)

project(TheInformer VERSION 0.1.0)

#This is temporarily needed due to a bug in Xcode 15:
if (CMAKE_CXX_COMPILER_ID MATCHES "AppleClang")
    if (NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS "15.0")
        add_compile_definitions(JUCE_SILENCE_XCODE_15_LINKER_WARNING=1)
        set(CMAKE_EXE_LINKER_FLAGS "-Wl,-ld_classic" CACHE INTERNAL "")
    endif ()
endif ()

#First, we'll add the CMake folder, incase we'll need to find_package later:
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMake")

#Compile commands, useful for some IDEs like VS-Code
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

#Minimum MacOS target, set globally
if (${CMAKE_SYSTEM_NAME} STREQUAL "iOS")
    set(CMAKE_OSX_DEPLOYMENT_TARGET 11.0 CACHE STRING "Minimum OS X deployment version" FORCE)

    #code signing to run on an iOS device:
    #    set(CMAKE_XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "iPhone Developer" CACHE STRING "" FORCE)
    #    set(CMAKE_XCODE_ATTRIBUTE_DEVELOPMENT_TEAM "XXXXXXXXXX" CACHE STRING "" FORCE)
else ()
    set(CMAKE_OSX_DEPLOYMENT_TARGET "10.11" CACHE STRING "Minimum OS X deployment version" FORCE)
endif ()

option(UniversalBinary "Build universal binary for mac" OFF)

if (UniversalBinary)
    set(CMAKE_OSX_ARCHITECTURES "x86_64;arm64" CACHE INTERNAL "")
endif ()

#static linking in Windows
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

#We 'require' that we need juce. If JUCE isn't found, it will revert to what's in
#CMake/Findjuce.cmake, where you can see how JUCE is brought in/configured
find_package(juce REQUIRED)

set (BaseTargetName TheInformer)

juce_add_plugin("${BaseTargetName}"
        # ICON_BIG ...                              # ICON_* arguments specify a path to an image file to use as an icon for the Standalone
        # ICON_SMALL ...
        COMPANY_NAME "Valerio Orlandini"
        IS_SYNTH FALSE
        NEEDS_MIDI_INPUT TRUE
        NEEDS_MIDI_OUTPUT FALSE
        IS_MIDI_EFFECT FALSE
        EDITOR_WANTS_KEYBOARD_FOCUS FALSE
        COPY_PLUGIN_AFTER_BUILD FALSE
        PLUGIN_MANUFACTURER_CODE Vorl
        PLUGIN_CODE Tinf
        FORMATS AU VST3 Standalone LV2
        PRODUCT_NAME "The Informer"
	    BUNDLE_ID "com.valeriorlandini.theinformer"
	    LV2URI "http://www.valeriorlandini.com/plugins/theinformer")
# Define the output directory for binaries
set(BINARY_OUTPUT_DIR ${CMAKE_BINARY_DIR}/binaries)

target_sources(${BaseTargetName} PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginEditor.cpp)

target_compile_definitions(${BaseTargetName}
        PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0)

target_link_libraries(${BaseTargetName} PRIVATE
	    juce_dsp
	    juce_osc
	    juce_audio_utils
	    juce_audio_devices
        juce_recommended_config_flags
        juce_recommended_lto_flags
        juce_recommended_warning_flags)

# Define the output directory for binaries
set(BINARY_OUTPUT_DIR ${CMAKE_BINARY_DIR}/binaries)

# Ensure the 'binaries' folder exists
file(MAKE_DIRECTORY ${BINARY_OUTPUT_DIR})

# Determine the build type (Release or Debug)
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(ARTIFACTS_DIR ${CMAKE_BINARY_DIR}/${BaseTargetName}_artefacts/Debug)
else()
    set(ARTIFACTS_DIR ${CMAKE_BINARY_DIR}/${BaseTargetName}_artefacts/Release)
endif()

file(GLOB_RECURSE ALL_FILES ${ARTIFACTS_DIR}/*)

# Filter out .lib and .exp files
list(FILTER ALL_FILES EXCLUDE REGEX ".*\\.(lib|exp)$")

# Copy the remaining files/folders to the 'binaries' folder
foreach(file ${ALL_FILES})
    get_filename_component(file_name ${file} NAME)  # Extract the file name
    get_filename_component(file_dir ${file} DIRECTORY)  # Extract the file directory

    # Calculate the relative path of the directory
    file(RELATIVE_PATH relative_path ${ARTIFACTS_DIR} ${file_dir})

    # Create the corresponding directory in 'binaries'
    file(MAKE_DIRECTORY ${BINARY_OUTPUT_DIR}/${relative_path})

    # Copy the file to the corresponding directory in 'binaries'
    add_custom_command(
        TARGET ${BaseTargetName}
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy ${file} ${BINARY_OUTPUT_DIR}/${relative_path}/${file_name}
        COMMENT "Copying ${file_name} to ${BINARY_OUTPUT_DIR}/${relative_path}"
    )
endforeach()
