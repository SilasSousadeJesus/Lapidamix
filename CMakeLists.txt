cmake_minimum_required(VERSION 3.22)

project(Lapidamix VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# JUCE entra como submódulo, fixado na versão 9.0.4 (D49).
add_subdirectory(external/JUCE)

# Pasta VST3 do usuário: não exige administrador (D52).
file(TO_CMAKE_PATH "$ENV{LOCALAPPDATA}/Programs/Common/VST3" LAPIDAMIX_VST3_DIR)

# Depois do primeiro uso real, não trocar os códigos: projetos salvos
# deixariam de achar o plugin (D51).
juce_add_plugin(Lapidamix
    COMPANY_NAME "Lapidamix"
    PRODUCT_NAME "Lapidamix"
    PLUGIN_MANUFACTURER_CODE Lpmx
    PLUGIN_CODE Lpd1
    FORMATS VST3
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    NEEDS_WEB_BROWSER FALSE
    NEEDS_CURL FALSE
    COPY_PLUGIN_AFTER_BUILD TRUE
    VST3_COPY_DIR "${LAPIDAMIX_VST3_DIR}")

target_sources(Lapidamix
    PRIVATE
        src/core/eq/EqBandFilter.cpp
        src/modules/EqModule.cpp
        src/plugin/PluginEditor.cpp
        src/plugin/PluginProcessor.cpp)

target_include_directories(Lapidamix
    PRIVATE
        src)

target_compile_definitions(Lapidamix
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0)

# Os nomes das bandas têm acentos: o compilador precisa ler o código como UTF-8.
target_compile_options(Lapidamix
    PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/utf-8>)

target_link_libraries(Lapidamix
    PRIVATE
        juce::juce_audio_utils
        juce::juce_dsp
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags)
