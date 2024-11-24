cmake_minimum_required(VERSION 3.19)

project(TheInformer VERSION 0.1.0)

# This is temporarily needed due to a bug in Xcode 15:
if (CMAKE_CXX_COMPILER_ID MATCHES "AppleClang")
    if (NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS "15.0")
        add_compile_definitions(JUCE_SILENCE_XCODE_15_LINKER_WARNING=1)
        set(CMAKE_EXE_LINKER_FLAGS "-Wl,-ld_classic" CACHE INTERNAL "")
    endif ()
endif ()

# Add the CMake folder
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMake")

# Compile commands
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Minimum MacOS target, set globally
if (${CMAKE_SYSTEM_NAME} STREQUAL "iOS")
    set(CMAKE_OSX_DEPLOYMENT_TARGET 11.0 CACHE STRING "Minimum OS X deployment version" FORCE)
else ()
    set(CMAKE_OSX_DEPLOYMENT_TARGET "10.11" CACHE STRING "Minimum OS X deployment version" FORCE)
endif ()

option(UniversalBinary "Build universal binary for mac" OFF)

if (UniversalBinary)
    set(CMAKE_OSX_ARCHITECTURES "x86_64;arm64" CACHE INTERNAL "")
endif ()

# Static linking in Windows
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

# Require JUCE
find_package(juce REQUIRED)

set(BaseTargetName TheInformer)
set(PluginName "The Informer")

juce_add_plugin("${BaseTargetName}"
        # ICON_BIG ...
        # ICON_SMALL ...
        COMPANY_NAME "Valerio Orlandini"
        IS_SYNTH FALSE
        NEEDS_MIDI_INPUT FALSE
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

target_sources(${BaseTargetName} PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginEditor.cpp
        Source/CustomLookAndFeel.cpp)

juce_add_binary_data(BinaryData SOURCES
        Resources/Logo.svg
        Resources/Font.ttf
)

target_compile_definitions(${BaseTargetName}
        PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0)

target_link_libraries(${BaseTargetName} PRIVATE
        BinaryData
	    juce_dsp
	    juce_osc
	    juce_audio_utils
	    juce_audio_devices
        juce_recommended_config_flags
        juce_recommended_lto_flags
        juce_recommended_warning_flags)
