project(TheInformer VERSION 0.1.0)

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
