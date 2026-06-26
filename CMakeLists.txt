cmake_minimum_required(VERSION 3.22)
project(LiveStageAI_2 VERSION 0.2.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(ArrangerCoreProof
    App/ArrangerCoreProof.cpp
    Engine/SoundFontEngine.cpp
    Engine/ArrangerEngine.cpp
    Engine/TimingEngine.cpp
)

target_include_directories(ArrangerCoreProof PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_definitions(ArrangerCoreProof PRIVATE NOMINMAX WIN32_LEAN_AND_MEAN)
