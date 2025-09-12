// SPDX-License-Identifier: GPL-3.0-or-later

#include <ModalSynth/PluginProcessor.hpp>
#include <ModalSynth/PluginEditor.hpp>

namespace modal::plugin {
//==============================================================================
    Editor::Editor(Processor& p, juce::AudioProcessorValueTreeState& pa) : AudioProcessorEditor(&p), processorRef(p), params{pa} {
        juce::ignoreUnused(processorRef);
        const int width = 800;
        const int height = JUCEApplicationBase::isStandaloneApp() ? 700 : 600;
        const double ratio = static_cast<double>(width) / static_cast<double>(height);
        setSize(width, height);

        setMouseClickGrabsKeyboardFocus(false);

        setResizable(true, true);
        constraints.setFixedAspectRatio(ratio);
        setConstrainer(&constraints);

        setLookAndFeel(&laf);

        controls.setup(params);
        sliders.setup(params);
        formant_controls.setup(params);
        exciter_controls.setup(params);
        addAndMakeVisible(controls);
        addAndMakeVisible(sliders);
        addAndMakeVisible(formant_controls);
        addAndMakeVisible(exciter_controls);

        if (JUCEApplicationBase::isStandaloneApp()) {
            addAndMakeVisible(keyboard);
        }
    }

    Editor::~Editor() = default;

//==============================================================================
    void Editor::paint(juce::Graphics&) {
        // (Our component is opaque, so we must completely fill the background with a solid colour)
        // g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
        //
        // g.setColour (juce::Colours::white);
        // g.setFont (15.0f);
        // g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
    }

    void Editor::resized() {
        // This is generally where you'll want to lay out the positions of any
        // subcomponents in your editor.

        Grid grid;

        using Track = Grid::TrackInfo;
        using Fr = Grid::Fr;
        using Item = GridItem;

        grid.templateRows = {Track{Fr{200}}, Track{Fr{400}}};
        grid.templateColumns = {Track{Fr{1}}, Track{Fr{1}}, Track{Fr{1}}};

        grid.items = {
                Item{controls}.withColumn({1, 4}),
                Item{exciter_controls}, Item{sliders}, Item{formant_controls}
        };

        if (JUCEApplicationBase::isStandaloneApp()) {
            grid.templateRows.add(Track{Fr{100}});
            grid.items.add(Item{keyboard}.withColumn({1, 4}).withRow({3, 3}));
        }

        grid.performLayout(getLocalBounds());

        laf.setWindowWidth(getLocalBounds().getWidth());
    }

    void Editor::Controls::setup(juce::AudioProcessorValueTreeState& plug_params) {

        setMouseClickGrabsKeyboardFocus(false);

        foldback_mode.setup(plug_params, "foldback_mode");
        foldback_point.setup(plug_params, "foldback_point");
        num_modes.setup(plug_params, "modes");
        detune.setup(plug_params, "detune");
        exponent.setup(plug_params, "exponent");
        falloff.setup(plug_params, "falloff");
        decay.setup(plug_params, "decay");

        addAndMakeVisible(foldback_mode);
        addAndMakeVisible(foldback_point);
        addAndMakeVisible(num_modes);
        addAndMakeVisible(detune);
        addAndMakeVisible(exponent);
        addAndMakeVisible(falloff);
        addAndMakeVisible(decay);
    }

    void Editor::Controls::paint(juce::Graphics& g) {
        g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
    }

    void Editor::Controls::resized() {
        juce::FlexBox fb;
        fb.flexWrap = juce::FlexBox::Wrap::wrap;
        fb.justifyContent = juce::FlexBox::JustifyContent::center;
        fb.alignContent = juce::FlexBox::AlignContent::stretch;
        fb.flexDirection = juce::FlexBox::Direction::row;
        fb.items = {
                FlexItem(num_modes).withFlex(1),
                FlexItem(foldback_mode).withFlex(1).withHeight(40).withAlignSelf(FlexItem::AlignSelf::center),
                FlexItem(foldback_point).withFlex(1),
                FlexItem(detune).withFlex(1),
                FlexItem(exponent).withFlex(1),
                FlexItem(falloff).withFlex(1),
                FlexItem(decay).withFlex(1),
        };

        fb.performLayout(getLocalBounds().reduced(10));
    }

    void Editor::Sliders::setup(juce::AudioProcessorValueTreeState& plug_params) {
        setMouseClickGrabsKeyboardFocus(false);

        slider1.setup(plug_params, "slider1");
        slider2.setup(plug_params, "slider2");

        dial1.setup(plug_params, "dial1");
        dial2.setup(plug_params, "dial2");

        l.setJustificationType(Justification::centred);
        l.setFont(Font{FontOptions{24}});

        addAndMakeVisible(slider1);
        addAndMakeVisible(slider2);

        addAndMakeVisible(dial1);
        addAndMakeVisible(dial2);
        addAndMakeVisible(l);

    }

    void Editor::Sliders::paint(juce::Graphics& g) {
        g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
    }

    void Editor::Sliders::resized() {
        juce::Grid grid;

        using Track = juce::Grid::TrackInfo;
        using Fr = juce::Grid::Fr;
        using Item = juce::GridItem;

        grid.templateRows = {Track{Fr{3}}, Track{Fr{8}}, Track{Fr{16}}};
        grid.templateColumns = {Track{Fr{1}}, Track{Fr{1}}};

        grid.items = {
                Item{l}.withColumn({1, 3}),
                Item{dial1}, Item{dial2},
                Item{slider1}, Item{slider2},
        };

        grid.performLayout(getLocalBounds().reduced(10));

    }

    void Editor::ExciterControls::setup(AudioProcessorValueTreeState& plug_params) {
        setMouseClickGrabsKeyboardFocus(false);

        exciter_mode.setup(plug_params, "exciter");
        exciter_rate.setup(plug_params, "exciter_rate");

        attack.setup(plug_params, "attack");
        release.setup(plug_params, "release");

        l.setJustificationType(Justification::centred);
        l.setFont(Font{FontOptions{24}});

        addAndMakeVisible(exciter_mode);
        addAndMakeVisible(exciter_rate);
        addAndMakeVisible(attack);
        addAndMakeVisible(release);
        addAndMakeVisible(l);
    }

    void Editor::ExciterControls::paint(Graphics& g) {
        g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
    }

    void Editor::ExciterControls::resized() {
        Grid grid;

        using Track = Grid::TrackInfo;
        using Fr = Grid::Fr;
        using Item = GridItem;

        grid.templateRows = {Track{Fr{1}}, Track{Fr{4}}, Track{Fr{4}}};
        grid.templateColumns = {Track{Fr{1}}, Track{Fr{1}}};

        grid.items = {
                Item{l}.withColumn({1, 3}),
                Item{exciter_mode}.withHeight(40).withAlignSelf(GridItem::AlignSelf::center), Item{exciter_rate},
                Item{attack}, Item{release}
        };

        grid.performLayout(getLocalBounds().reduced(10));
    }

    void Editor::FormantControls::setup(AudioProcessorValueTreeState& plug_params) {
        setMouseClickGrabsKeyboardFocus(false);

        x.setup(plug_params, "formant_x");
        y.setup(plug_params, "formant_y");
        len.setup(plug_params, "formant_len");
        mix.setup(plug_params, "formant_mix");

        l.setJustificationType(Justification::centred);
        l.setFont(Font{FontOptions{24}});

        addAndMakeVisible(x);
        addAndMakeVisible(y);
        addAndMakeVisible(len);
        addAndMakeVisible(mix);
        addAndMakeVisible(l);
    }

    void Editor::FormantControls::paint(Graphics& g) {
        g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
    }

    void Editor::FormantControls::resized() {
        Grid grid;

        using Track = Grid::TrackInfo;
        using Fr = Grid::Fr;
        using Item = GridItem;

        grid.templateRows = {Track{Fr{1}}, Track{Fr{4}}, Track{Fr{4}}};
        grid.templateColumns = {Track{Fr{1}}, Track{Fr{1}}};

        grid.items = {
                Item{l}.withColumn({1, 3}),
                Item{x}, Item{y},
                Item{len}, Item{mix}
        };

        grid.performLayout(getLocalBounds().reduced(10));
    }
}
