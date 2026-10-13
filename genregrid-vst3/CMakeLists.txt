cmake_minimum_required(VERSION 3.22)
project(GenreGridMidiVST3 VERSION 0.1.0)

include(FetchContent)
FetchContent_Declare(JUCE
  GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
  GIT_TAG 8.0.8
  GIT_SHALLOW TRUE)
FetchContent_MakeAvailable(JUCE)

juce_add_plugin(GenreGridMidi
  COMPANY_NAME "GenreGrid"
  BUNDLE_ID "com.genregrid.midigenerator"
  PLUGIN_MANUFACTURER_CODE Gnrg
  PLUGIN_CODE GgM1
  FORMATS VST3
  PRODUCT_NAME "GenreGrid MIDI"
  DESCRIPTION "MIDI phrase variation generator"
  IS_SYNTH FALSE
  IS_MIDI_EFFECT TRUE
  NEEDS_MIDI_INPUT FALSE
  NEEDS_MIDI_OUTPUT TRUE
  EDITOR_WANTS_KEYBOARD_FOCUS TRUE
  COPY_PLUGIN_AFTER_BUILD FALSE)

target_sources(GenreGridMidi PRIVATE
  Source/PluginProcessor.cpp
  Source/PluginProcessor.h
  Source/PluginEditor.cpp
  Source/PluginEditor.h)

target_compile_features(GenreGridMidi PRIVATE cxx_std_17)
target_compile_definitions(GenreGridMidi PUBLIC
  JUCE_WEB_BROWSER=0
  JUCE_USE_CURL=0
  JUCE_VST3_CAN_REPLACE_VST2=0)
target_link_libraries(GenreGridMidi PRIVATE
  juce::juce_audio_utils
  juce::juce_audio_processors
  juce::juce_gui_extra
  PUBLIC juce::juce_recommended_config_flags
         juce::juce_recommended_lto_flags
         juce::juce_recommended_warning_flags)
