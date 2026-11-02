cmake_minimum_required(VERSION 3.22)

# Ferramenta de teste, separada do plugin (D84). Ver README.md nesta pasta.
project(MedidorVoz VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

get_filename_component(LAPIDAMIX_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../.." ABSOLUTE)
add_subdirectory("${LAPIDAMIX_ROOT}/external/JUCE" juce_bin)

# Usa o código real da análise, sem cópia: a medição acompanha qualquer mudança no método.
function(add_meter name source analyzer)
    juce_add_console_app(${name} PRODUCT_NAME "${name}")
    target_sources(${name}
        PRIVATE
            ${source}
            "${analyzer}"
            "${LAPIDAMIX_ROOT}/src/core/analysis/YinPitchDetector.cpp"
            "${LAPIDAMIX_ROOT}/src/core/analysis/AnalysisResult.cpp"
            "${LAPIDAMIX_ROOT}/src/core/correction/CorrectionRules.cpp")
    target_include_directories(${name} PRIVATE "${LAPIDAMIX_ROOT}/src")
    target_compile_definitions(${name}
        PRIVATE
            JUCE_WEB_BROWSER=0
            JUCE_USE_CURL=0
            LAPIDAMIX_ROOT="${LAPIDAMIX_ROOT}")
    target_compile_options(${name} PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/utf-8>)
    target_link_libraries(${name} PRIVATE juce::juce_audio_formats juce::juce_dsp juce::juce_data_structures)
endfunction()

set(analyzer "${LAPIDAMIX_ROOT}/src/core/analysis/VoiceAnalyzer.cpp")

add_meter(medidor medidor.cpp "${analyzer}")
add_meter(resumo resumo.cpp "${analyzer}")
add_meter(banco banco.cpp "${analyzer}")
add_meter(origem origem.cpp "${analyzer}")

# O banco de novo, sem o limite de 6 vezes a fundamental, para ver quanto esse limite custa (D83).
# A cópia é gerada na pasta de compilação; o código do plugin não muda.
file(READ "${analyzer}" analyzer_code)
string(REPLACE "minimumHarmonicNumber = 6.0f" "minimumHarmonicNumber = 0.0f" analyzer_without_limit "${analyzer_code}")
if(analyzer_without_limit STREQUAL analyzer_code)
    message(FATAL_ERROR "Limite de busca nao encontrado em VoiceAnalyzer.cpp")
endif()
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/VoiceAnalyzerSemLimite.cpp" "${analyzer_without_limit}")
add_meter(banco_sem_limite banco.cpp "${CMAKE_CURRENT_BINARY_DIR}/VoiceAnalyzerSemLimite.cpp")
