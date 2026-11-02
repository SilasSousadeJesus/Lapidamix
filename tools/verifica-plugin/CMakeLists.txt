cmake_minimum_required(VERSION 3.22)

# Ferramenta de teste, separada do plugin (D84). Ver README.md nesta pasta.
project(VerificaPlugin VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

get_filename_component(LAPIDAMIX_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../.." ABSOLUTE)
add_subdirectory("${LAPIDAMIX_ROOT}/external/JUCE" juce_bin)

juce_add_console_app(verifica PRODUCT_NAME "verifica")

file(GLOB plugin_sources CONFIGURE_DEPENDS
    "${LAPIDAMIX_ROOT}/src/core/analysis/*.cpp"
    "${LAPIDAMIX_ROOT}/src/core/correction/*.cpp"
    "${LAPIDAMIX_ROOT}/src/core/eq/*.cpp"
    "${LAPIDAMIX_ROOT}/src/modules/*.cpp"
    "${LAPIDAMIX_ROOT}/src/plugin/*.cpp")

target_sources(verifica PRIVATE verifica.cpp ${plugin_sources})
target_include_directories(verifica PRIVATE "${LAPIDAMIX_ROOT}/src")
target_compile_definitions(verifica
    PRIVATE
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_MODAL_LOOPS_PERMITTED=1
        JucePlugin_Name="Lapidamix"
        LAPIDAMIX_ROOT="${LAPIDAMIX_ROOT}")
target_compile_options(verifica PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/utf-8>)
target_link_libraries(verifica PRIVATE juce::juce_audio_utils juce::juce_dsp)
