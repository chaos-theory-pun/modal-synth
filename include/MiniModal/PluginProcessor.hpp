// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

#include <dsp/mini_modal_synth.hpp>
#include <dsp/control.hpp>

namespace modal::plugin {
//==============================================================================
    class MiniProcessor final : public juce::AudioProcessor, juce::ValueTree::Listener {
     public:
        //==============================================================================
        MiniProcessor();

        ~MiniProcessor() override;

        //==============================================================================
        void prepareToPlay(double sampleRate, int samplesPerBlock) override;

        void releaseResources() override;

        bool isBusesLayoutSupported(const BusesLayout& layouts) const override;

        void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

        using AudioProcessor::processBlock;

        //==============================================================================
        juce::AudioProcessorEditor* createEditor() override;

        bool hasEditor() const override;

        //==============================================================================
        const juce::String getName() const override;

        bool acceptsMidi() const override;

        bool producesMidi() const override;

        bool isMidiEffect() const override;

        double getTailLengthSeconds() const override;

        //==============================================================================
        int getNumPrograms() override;

        int getCurrentProgram() override;

        void setCurrentProgram(int index) override;

        const juce::String getProgramName(int index) override;

        void changeProgramName(int index, const juce::String& newName) override;

        //==============================================================================
        void getStateInformation(juce::MemoryBlock& destData) override;

        void setStateInformation(const void* data, int sizeInBytes) override;

        void valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged,
                                      const juce::Identifier& property) override;

        juce::MidiKeyboardState keyboard_state;

     private:
        //==============================================================================
        juce::AudioProcessorValueTreeState params;
        std::atomic_bool params_changed = true;

        std::array<dsp::synth::MiniModalSynth<40>, 16> modal_synths;
        dsp::PolyController<dsp::synth::MiniModalSynth<40>, 16> controller;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MiniProcessor)
    };
}