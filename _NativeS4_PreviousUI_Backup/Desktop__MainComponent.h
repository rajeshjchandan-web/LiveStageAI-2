#pragma once

#include <JuceHeader.h>
#include "Engine/SoundFontEngine.h"
#include "Engine/RealtimeArrangerEngine.h"

class MainComponent final : public juce::Component,
                            private juce::Timer
{
public:
    MainComponent();
    ~MainComponent() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    void timerCallback() override;
    void initialiseAudioEngine();
    void updateTransportTimer();
    void refreshReadouts();
    void stopEverything();

    void configureButton(juce::TextButton& button,
                         std::function<void()> callback,
                         bool accent = false);

    SoundFontEngine soundFont;
    RealtimeArrangerEngine arranger;

    bool engineReady = false;

    juce::Label titleLabel;
    juce::Label engineStatusLabel;

    juce::Label sectionValue;
    juce::Label chordValue;
    juce::Label barBeatValue;
    juce::Label tempoValue;

    juce::Slider tempoSlider;
    juce::ComboBox chordRootBox;
    juce::ToggleButton minorToggle { "Minor" };

    juce::TextButton syncStartButton { "SYNC START" };
    juce::TextButton startButton     { "START" };
    juce::TextButton stopButton      { "STOP" };
    juce::TextButton mainAButton     { "MAIN A" };
    juce::TextButton mainBButton     { "MAIN B" };
    juce::TextButton mainCButton     { "MAIN C" };
    juce::TextButton fillBButton     { "FILL B" };
    juce::TextButton fillCButton     { "FILL C" };
    juce::TextButton endingButton    { "ENDING A" };
    juce::TextButton panicButton     { "PANIC" };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
