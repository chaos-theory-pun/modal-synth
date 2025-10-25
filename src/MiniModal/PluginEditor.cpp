// SPDX-License-Identifier: GPL-3.0-or-later

#include <MiniModal/PluginProcessor.hpp>
#include <MiniModal/PluginEditor.hpp>

namespace modal::plugin {
//==============================================================================
    MiniEditor::MiniEditor(MiniProcessor& p, juce::AudioProcessorValueTreeState& pa) : AudioProcessorEditor(&p), processorRef(p), params{pa} {
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
        spectrum_controls.setup(params);
        exciter_controls.setup(params);
        macro_controls.setup(params);
        addAndMakeVisible(controls);
        addAndMakeVisible(spectrum_controls);
        addAndMakeVisible(exciter_controls);
        addAndMakeVisible(macro_controls);

        if (JUCEApplicationBase::isStandaloneApp()) {
            addAndMakeVisible(keyboard);
        }
    }

    MiniEditor::~MiniEditor() = default;

//==============================================================================
    void MiniEditor::paint(juce::Graphics&) {
        // (Our component is opaque, so we must completely fill the background with a solid colour)
        // g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
        //
        // g.setColour (juce::Colours::white);
        // g.setFont (15.0f);
        // g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
    }

    void MiniEditor::resized() {
        // This is generally where you'll want to lay out the positions of any
        // subcomponents in your editor.

        Grid grid;

        using Track = Grid::TrackInfo;
        using Fr = Grid::Fr;
        using Item = GridItem;

        grid.templateRows = {Track{Fr{200}}, Track{Fr{400}}};
        grid.templateColumns = {Track{Fr{2}}, Track{Fr{2}}, Track{Fr{2}}};

        grid.items = {
            Item{controls}.withColumn({1, 3}),
            Item{macro_controls}.withColumn({3, 4}).withRow({1, 3}),
            Item{exciter_controls}, Item{spectrum_controls}
        };

        if (JUCEApplicationBase::isStandaloneApp()) {
            grid.templateRows.add(Track{Fr{100}});
            grid.items.add(Item{keyboard}.withColumn({1, 4}).withRow({3, 3}));
        }

        grid.performLayout(getLocalBounds());

        laf.setWindowWidth(getLocalBounds().getWidth());
    }

    void MiniEditor::Controls::setup(juce::AudioProcessorValueTreeState& plug_params) {

        setMouseClickGrabsKeyboardFocus(false);

        foldback_mode.setup(plug_params, "foldback_mode");
        foldback_point.setup(plug_params, "foldback_point");
        num_modes.setup(plug_params, "modes");
        decay.setup(plug_params, "decay");
        fb_amount.setup(plug_params, "fb_amt");
        fb_time.setup(plug_params, "fb_time");

        addAndMakeVisible(foldback_mode);
        addAndMakeVisible(foldback_point);
        addAndMakeVisible(num_modes);
        addAndMakeVisible(decay);
        addAndMakeVisible(fb_amount);
        addAndMakeVisible(fb_time);
    }

    void MiniEditor::Controls::paint(juce::Graphics& g) {
        g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
    }

    void MiniEditor::Controls::resized() {
        juce::FlexBox fb;
        fb.flexWrap = juce::FlexBox::Wrap::wrap;
        fb.justifyContent = juce::FlexBox::JustifyContent::center;
        fb.alignContent = juce::FlexBox::AlignContent::stretch;
        fb.flexDirection = juce::FlexBox::Direction::row;
        fb.items = {
                FlexItem(num_modes).withFlex(1),
                FlexItem(foldback_mode).withFlex(1).withHeight(40).withAlignSelf(FlexItem::AlignSelf::center),
                FlexItem(foldback_point).withFlex(1),
                FlexItem(decay).withFlex(1),
                FlexItem(fb_amount).withFlex(1),
                FlexItem(fb_time).withFlex(1)
        };

        fb.performLayout(getLocalBounds().reduced(10));
    }

    void MiniEditor::SpectrumControls::setup(juce::AudioProcessorValueTreeState& plug_params) {
        setMouseClickGrabsKeyboardFocus(false);

        detune.setup(plug_params, "detune");
        exponent.setup(plug_params, "exponent");
        falloff.setup(plug_params, "falloff");
        even_gain.setup(plug_params, "even_gain");


        l.setJustificationType(Justification::centred);
        l.setFont(Font{FontOptions{24}});

        addAndMakeVisible(detune);
        addAndMakeVisible(exponent);
        addAndMakeVisible(falloff);
        addAndMakeVisible(even_gain);
        addAndMakeVisible(l);

    }

    void MiniEditor::SpectrumControls::paint(juce::Graphics& g) {
        g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
    }

    void MiniEditor::SpectrumControls::resized() {
        juce::Grid grid;

        using Track = juce::Grid::TrackInfo;
        using Fr = juce::Grid::Fr;
        using Item = juce::GridItem;

        grid.templateRows = {Track{Fr{1}}, Track{Fr{4}}, Track{Fr{4}}};
        grid.templateColumns = {Track{Fr{1}}, Track{Fr{1}}};

        grid.items = {
                Item{l}.withColumn({1, 3}),
                Item{detune}, Item{exponent},
                Item{falloff}, Item{even_gain},
        };

        grid.performLayout(getLocalBounds().reduced(10));

    }

    void MiniEditor::ExciterControls::setup(AudioProcessorValueTreeState& plug_params) {
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

    void MiniEditor::ExciterControls::paint(Graphics& g) {
        g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
    }

    void MiniEditor::ExciterControls::resized() {
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

    void MiniEditor::MacroControls::setup(AudioProcessorValueTreeState& plug_params) {
        macro_1.setup();
        addAndMakeVisible(macro_1);

        macro_1_dial.setup(plug_params, "macro_control_1");
        addAndMakeVisible(macro_1_dial);

        macro_2.setup();
        addAndMakeVisible(macro_2);

        macro_2_dial.setup(plug_params, "macro_control_2");
        addAndMakeVisible(macro_2_dial);
    }

    void MiniEditor::MacroControls::resized() {
        juce::FlexBox fb;
        fb.flexWrap = juce::FlexBox::Wrap::wrap;
        fb.justifyContent = juce::FlexBox::JustifyContent::center;
        fb.alignContent = juce::FlexBox::AlignContent::stretch;
        fb.flexDirection = juce::FlexBox::Direction::column;
        fb.items = {
            FlexItem(macro_1_dial).withFlex(2),
            FlexItem(macro_1).withFlex(4),
            FlexItem(macro_2_dial).withFlex(2),
            FlexItem(macro_2).withFlex(4),
        };

        fb.performLayout(getLocalBounds().reduced(10));
    }
}
