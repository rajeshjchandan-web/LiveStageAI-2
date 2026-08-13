#pragma once
#include <JuceHeader.h>
#include "Engine/SoundFontEngine.h"

class MainComponent final : public juce::Component, private juce::Timer
{
public:
    MainComponent();
    ~MainComponent() override;
    void paint(juce::Graphics&) override;
    void resized() override;

private:
    void timerCallback() override;
    void initialiseAudioEngine();
    void playAudioTest();
    void stopAllAudio();

    SoundFontEngine soundFont;
    bool engineReady = false;

    juce::Label titleLabel;
    juce::Label statusLabel;
    juce::TextButton audioTestButton { "TEST AUDIO" };
    juce::TextButton panicButton { "PANIC / ALL NOTES OFF" };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
