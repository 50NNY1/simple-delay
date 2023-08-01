cmake_minimum_required(VERSION 3.15)
project(AUDIO_PLUGIN_EXAMPLE VERSION 0.0.1)
add_subdirectory(JUCE) # If you've put JUCE in a subdirectory called JUCE

juce_add_plugin(AudioPluginExample

    # ICON_BIG ...
    # ICON_SMALL ...
    COMPANY_NAME "Sonny Holland"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS TRUE
    COPY_PLUGIN_AFTER_BUILD TRUE
    PLUGIN_MANUFACTURER_CODE Juce
    PLUGIN_CODE Dem0
    FORMATS AU VST3 Standalone
    PRODUCT_NAME "Simple Delay")

# juce_generate_juce_header(AudioPluginExample)
target_sources(AudioPluginExample
    PRIVATE
    PluginEditor.cpp
    PluginProcessor.cpp)

target_compile_definitions(AudioPluginExample
    PUBLIC

    # JUCE_WEB_BROWSER and JUCE_USE_CURL would be on by default, but you might not need them.
    JUCE_WEB_BROWSER=0 # If you remove this, add `NEEDS_WEB_BROWSER TRUE` to the `juce_add_plugin` call
    JUCE_USE_CURL=0 # If you remove this, add `NEEDS_CURL TRUE` to the `juce_add_plugin` call
    JUCE_VST3_CAN_REPLACE_VST2=0)

# juce_add_binary_data(AudioPluginData SOURCES ...)
target_link_libraries(AudioPluginExample
    PRIVATE

    # AudioPluginData           # If we'd created a binary data target, we'd link to it here
    juce::juce_audio_utils
    PUBLIC
    juce::juce_recommended_config_flags
    juce::juce_recommended_lto_flags
    juce::juce_recommended_warning_flags)
