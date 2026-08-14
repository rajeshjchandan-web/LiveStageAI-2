cmake_minimum_required(VERSION 3.22)
project(LiveStageAI_2 VERSION 2.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/JUCE/CMakeLists.txt")
    message(FATAL_ERROR "Local JUCE source not found in ${CMAKE_CURRENT_SOURCE_DIR}/JUCE")
endif()

add_subdirectory(JUCE)

add_executable(ArrangerCoreProof
    App/ArrangerCoreProof.cpp
    Engine/SoundFontEngine.cpp
    Engine/ArrangerEngine.cpp
    Engine/TimingEngine.cpp
)
target_include_directories(ArrangerCoreProof PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_definitions(ArrangerCoreProof PRIVATE NOMINMAX WIN32_LEAN_AND_MEAN)

juce_add_gui_app(LiveStageAI
    PRODUCT_NAME "LiveStage AI"
    COMPANY_NAME "LiveStage AI"
    BUNDLE_ID "com.livestageai.app"
)

target_sources(LiveStageAI PRIVATE
    Desktop/Main.cpp
    Desktop/MainComponent.cpp
    Desktop/MainComponent.h
    Engine/SoundFontEngine.cpp
    Engine/SoundFontEngine.h
    Engine/RealtimeArrangerEngine.cpp
    Engine/RealtimeArrangerEngine.h
)

target_include_directories(LiveStageAI PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_compile_definitions(LiveStageAI PRIVATE
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_REPORT_APP_USAGE=0
    NOMINMAX
    WIN32_LEAN_AND_MEAN
)

target_link_libraries(LiveStageAI PRIVATE
    juce::juce_gui_extra
    juce::juce_gui_basics
    juce::juce_graphics
    juce::juce_events
    juce::juce_core
)

juce_generate_juce_header(LiveStageAI)

add_custom_command(TARGET LiveStageAI POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/RuntimeDLLs"
        "$<TARGET_FILE_DIR:LiveStageAI>"
    COMMAND ${CMAKE_COMMAND} -E make_directory
        "$<TARGET_FILE_DIR:LiveStageAI>/SoundFonts"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_CURRENT_SOURCE_DIR}/SoundFonts/FluidR3_GM.sf2"
        "$<TARGET_FILE_DIR:LiveStageAI>/SoundFonts/FluidR3_GM.sf2"
)
