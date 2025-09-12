# like https://github.com/juce-framework/JUCE/tree/master/examples/CMake/AudioPlugin

cmake_minimum_required(VERSION 3.22)

set(CMAKE_CXX_STANDARD 20)

# build universal plugin
set(CMAKE_OSX_ARCHITECTURES arm64 x86_64)
set_property(GLOBAL PROPERTY USE_FOLDERS YES)

set(MODAL_NUM_TYPE float CACHE STRING "set the float type used for DSP calculations, defaults to `float`")
option(MODAL_INSTALL_PLUGIN "install the plugins to the default user plugin directories after every build, defaults to `off`")
option(MODAL_DEBUG_UI "build and enable a JUCE UI inspector, defaults to `off`")
option(MODAL_BUILD_DOCS "build docs using Doxygen, adds target ModalSynthDocs, defaults to `on`" ON)
option(MODAL_BUILD_TESTS "build docs using Catch2, adds target ModalSynthTests, defaults to `on`" ON)

project(ModalSynth VERSION 0.0.1)

if(DEFINED CMAKE_BUILD_TYPE)
    string(TOUPPER ${CMAKE_BUILD_TYPE} $BUILD_TYPE)
else()
    set($BUILD_TYPE "DEFAULT")
endif()

add_subdirectory(libs/JUCE SYSTEM)
add_subdirectory(libs/melatonin_inspector SYSTEM)
add_subdirectory(libs/randutils SYSTEM)

include(CMakeHelpers.txt)

set(common_sources
        include/ui/BoundCombobox.hpp
        src/ui/BoundCombobox.cpp
        include/ui/BoundSlider.hpp
        src/ui/BoundSlider.cpp
        include/ui/LookAndFeel.hpp

        include/dsp/dsp.hpp
        include/dsp/bonus.hpp
        src/dsp/bonus.cpp
        include/dsp/control.hpp
        include/dsp/filters.hpp
        src/dsp/filters.cpp
        include/dsp/formant.hpp
        src/dsp/formant.cpp
        include/dsp/mod.hpp
        src/dsp/mod.cpp
        include/dsp/modal_synth.hpp
        include/dsp/mini_modal_synth.hpp
        include/dsp/osc.hpp
        src/dsp/osc.cpp
        include/dsp/resonator.hpp
        src/dsp/resonator.cpp
)

set(big_modal_sources
        include/ModalSynth/PluginEditor.hpp
        src/ModalSynth/PluginEditor.cpp
        include/ModalSynth/PluginProcessor.hpp
        src/ModalSynth/PluginProcessor.cpp
)

add_synth(ModalSynthPlug Mdes "Modal synthesiser" big_modal_sources)

set(mini_modal_sources
        include/MiniModal/PluginEditor.hpp
        src/MiniModal/PluginEditor.cpp
        include/MiniModal/PluginProcessor.hpp
        src/MiniModal/PluginProcessor.cpp
)

add_synth(MiniModalPlug Mimo "MiniModal" mini_modal_sources)


if (MODAL_BUILD_DOCS)
    find_package(Doxygen)
    if (Doxygen_FOUND)
        set(DOXYGEN_SORT_BY_SCOPE_NAME YES)
        set(DOXYGEN_EXTRACT_ALL YES)
        set(DOXYGEN_SHOW_FILES NO)
        set(DOXYGEN_USE_MDFILE_AS_MAINPAGE README.md)
        set(DOXYGEN_EXCLUDE libs)
        set(DOXYGEN_EXCLUDE_SYMBOLS modal::plugin)

    #    for doxygen-awesome-css
        set(DOXYGEN_GENERATE_TREEVIEW YES)
        set(DOXYGEN_DISABLE_INDEX NO)
        set(DOXYGEN_FULL_SIDEBAR NO)
        set(DOXYGEN_HTML_EXTRA_STYLESHEET libs/doxygen-awesome-css/doxygen-awesome.css libs/doxygen-awesome-css/doxygen-awesome-sidebar-only.css)
        set(DOXYGEN_HTML_COLORSTYLE LIGHT)

        doxygen_add_docs(ModalSynthDocs)
    endif ()
endif ()

if (MODAL_BUILD_TESTS)
    add_subdirectory(libs/catch2 SYSTEM)
    add_executable(ModalSynthTests
            tests/start.cpp
            tests/dsp_bonus.cpp)
    target_link_libraries(ModalSynthTests PRIVATE Catch2::Catch2WithMain ModalSynthPlug)
endif ()