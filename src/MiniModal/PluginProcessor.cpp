// SPDX-License-Identifier: GPL-3.0-or-later

#include <MiniModal/PluginProcessor.hpp>
#include <MiniModal/PluginEditor.hpp>

namespace modal::plugin {
//==============================================================================
    MiniProcessor::MiniProcessor()
            : AudioProcessor(BusesProperties()
#if !JucePlugin_IsMidiEffect
#if !JucePlugin_IsSynth
                                     .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
#endif
                                     .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
    ), params{*this, nullptr, juce::Identifier("MiniModal"), {
            std::make_unique<juce::AudioParameterFloat>("even_gain", "Even Mode Amplitudes", 0, 1, 1),
            std::make_unique<juce::AudioParameterChoice>("foldback_mode", "Foldback Mode",
                                                         juce::StringArray{"Normal", "Undertones", "Foldback"}, 0),
            std::make_unique<juce::AudioParameterFloat>("foldback_point", "Foldback Point", 20, 20000, 1600),
            std::make_unique<juce::AudioParameterChoice>("exciter", "Exciter",
                                                         juce::StringArray{"Pick", "Blow", "Impulses"}, 0),
            std::make_unique<juce::AudioParameterFloat>("exciter_rate", "Exciter Rate Divider", 1, 100, 4),
            std::make_unique<juce::AudioParameterFloat>("attack", "Attack", 0, 5, 0.5),
            std::make_unique<juce::AudioParameterFloat>("release", "Release", 0, 5, 0.5),
            std::make_unique<juce::AudioParameterInt>("modes", "Mode Count", 1, 40, 40),
            std::make_unique<juce::AudioParameterFloat>("detune", "Mode Detune Linear", -0.06, 2, 0),
            std::make_unique<juce::AudioParameterFloat>("exponent", "Mode Detune Exponent", 0.1, 10, 1),
            std::make_unique<juce::AudioParameterFloat>("falloff", "Falloff Exponent", 0, 3, 1),
            std::make_unique<juce::AudioParameterFloat>("decay", "Decay", NormalisableRange<float>{0.1f, 5.f}, 1.f),
            std::make_unique<juce::AudioParameterFloat>("macro_dial", "Macro Dial", NormalisableRange<float>{0.f, 1.f}, 0.5f, AudioParameterFloatAttributes().withMeta(true))
    }},  mediator { *this }, controller{modal_synths} {
        params.state.addListener(this);
    }

    MiniProcessor::~MiniProcessor() = default;

//==============================================================================
    const juce::String MiniProcessor::getName() const {
        return JucePlugin_Name;
    }

    bool MiniProcessor::acceptsMidi() const {
#if JucePlugin_WantsMidiInput
        return true;
#else
        return false;
#endif
    }

    bool MiniProcessor::producesMidi() const {
#if JucePlugin_ProducesMidiOutput
        return true;
#else
        return false;
#endif
    }

    bool MiniProcessor::isMidiEffect() const {
#if JucePlugin_IsMidiEffect
        return true;
#else
        return false;
#endif
    }

    double MiniProcessor::getTailLengthSeconds() const {
        return 0.0;
    }

    int MiniProcessor::getNumPrograms() {
        return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
        // so this should be at least 1, even if you're not really implementing programs.
    }

    int MiniProcessor::getCurrentProgram() {
        return 0;
    }

    void MiniProcessor::setCurrentProgram(int index) {
        juce::ignoreUnused(index);
    }

    const juce::String MiniProcessor::getProgramName(int index) {
        juce::ignoreUnused(index);
        return {};
    }

    void MiniProcessor::changeProgramName(int index, const juce::String& newName) {
        juce::ignoreUnused(index, newName);
    }

//==============================================================================
    void MiniProcessor::prepareToPlay(double sampleRate, int samplesPerBlock) {
        for (auto& m: modal_synths) {
            m.set_sample_rate(static_cast<dsp::num>(sampleRate));
        }
        juce::ignoreUnused(samplesPerBlock);
    }

    void MiniProcessor::releaseResources() {
        // When playback stops, you can use this as an opportunity to free up any
        // spare memory, etc.
    }

    bool MiniProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const {
#if JucePlugin_IsMidiEffect
        juce::ignoreUnused (layouts);
        return true;
#else
        // This is the place where you check if the layout is supported.
        // In this template code we only support mono or stereo.
        // Some plugin hosts, such as certain GarageBand versions, will only
        // load plugins that support stereo bus layouts.
        if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
            && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
            return false;

        // This checks if the input layout matches the output layout
#if !JucePlugin_IsSynth
        if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
            return false;
#endif

        return true;
#endif
    }

    void MiniProcessor::processBlock(juce::AudioBuffer<float>& buffer,
                                                 juce::MidiBuffer& midiMessages) {

        keyboard_state.processNextMidiBuffer(midiMessages, 0, buffer.getNumSamples(), true);

        for (const auto& metadata: midiMessages) {
            auto m = metadata.getMessage();
            if (m.isNoteOn()) {
                params_changed = true;
                controller.key_down(m.getNoteNumber(), m.getFloatVelocity());
            } else if (m.isNoteOff()) {
                controller.key_up(m.getNoteNumber());
            }
        }

        mediator.set_values(*params.getRawParameterValue("macro_dial"));

        if (params_changed) {
            params_changed = false;
            auto exciter_mode = static_cast<dsp::synth::MiniModalExiterKind>(dynamic_cast<juce::AudioParameterChoice*>(params.getParameter(
                    "exciter"))->
                    getIndex());
            auto foldback_mode = static_cast<dsp::synth::MiniModalFoldbackKind>(
                    dynamic_cast<juce::AudioParameterChoice*>(params.getParameter("foldback_mode"))->getIndex());

            for (auto& m: modal_synths) {
                m.set_env_params(
                        *params.getRawParameterValue("attack"),
                        *params.getRawParameterValue("release")
                );
                bool changed = m.set_params(
                        (size_t) *params.getRawParameterValue("modes"),
                        *params.getRawParameterValue("detune"),
                        *params.getRawParameterValue("exponent"),
                        *params.getRawParameterValue("exciter_rate"),
                        *params.getRawParameterValue("decay"),
                        *params.getRawParameterValue("falloff"),
                        *params.getRawParameterValue("even_gain")
                );
                m.set_exciter(exciter_mode);
                changed |= m.set_foldback_settings(foldback_mode,
                                                   params.getRawParameterValue("foldback_point")->load());

                if (changed) {
                    m.update_mode_coefficients();
                }
            }

        }

        juce::ScopedNoDenormals noDenormals;
        auto totalNumInputChannels = getTotalNumInputChannels();
        auto totalNumOutputChannels = getTotalNumOutputChannels();

        for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i) {
            buffer.clear(i, 0, buffer.getNumSamples());
        }

        for (int i = 0; i < buffer.getNumSamples(); i++) {
            dsp::num out = 0;
            for (auto& m: modal_synths) {
                out += m.tick();
            }
            using namespace dsp; // for _nm literal
            out *= 0.1_nm;

            for (int channel = 0; channel < totalNumOutputChannels; ++channel) {
                buffer.setSample(channel, i, out);
            }
        }

    }

//==============================================================================
    bool MiniProcessor::hasEditor() const {
        return true; // (change this to false if you choose to not supply an editor)
    }

    juce::AudioProcessorEditor* MiniProcessor::createEditor() {
        return new MiniEditor(*this, params);
    }

//==============================================================================
    void MiniProcessor::getStateInformation(juce::MemoryBlock& destData) {
        juce::ValueTree state {"MiniModalSettings"};
        state.addChild(params.copyState(), -1, nullptr);
        state.addChild(mediator.dump_state(), -1, nullptr);

        const std::unique_ptr<juce::XmlElement> xml(state.createXml());
        copyXmlToBinary(*xml, destData);
    }

    void MiniProcessor::setStateInformation(const void* data, const int sizeInBytes) {
        const std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

        if (xmlState != nullptr) {
            const auto state = juce::ValueTree::fromXml(*xmlState);

            {
                const auto param_state = state.getChild(0);
                if (param_state.hasType(params.state.getType())) {
                    params.replaceState(param_state);
                }
            }

            {
                const auto macro_state = state.getChild(1);
                mediator.load_state(macro_state);
            }
        }
    }

    void MiniProcessor::valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged,
                                                             const juce::Identifier& property) {
        juce::ignoreUnused(treeWhosePropertyHasChanged, property);
        params_changed = true;
    }

}

//==============================================================================
// This creates new instances of the plugin.
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() {
    return new modal::plugin::MiniProcessor();
}
