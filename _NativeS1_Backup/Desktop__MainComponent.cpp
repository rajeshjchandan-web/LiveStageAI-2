#include "MainComponent.h"

namespace
{
const auto bg = juce::Colour::fromRGB(10,15,25);
const auto panel = juce::Colour::fromRGB(20,27,41);
const auto stroke = juce::Colour::fromRGB(50,64,88);
const auto accent = juce::Colour::fromRGB(61,194,255);
const auto success = juce::Colour::fromRGB(59,214,126);
const auto text = juce::Colour::fromRGB(236,242,250);
const auto muted = juce::Colour::fromRGB(147,160,182);
}

MainComponent::MainComponent()
{
    setOpaque(true);

    titleLabel.setText("LiveStage AI", juce::dontSendNotification);
    titleLabel.setFont(juce::Font(juce::FontOptions(30.0f, juce::Font::bold)));
    titleLabel.setColour(juce::Label::textColourId, text);
    addAndMakeVisible(titleLabel);

    statusLabel.setColour(juce::Label::textColourId, muted);
    addAndMakeVisible(statusLabel);

    audioTestButton.onClick = [this] { playAudioTest(); };
    panicButton.onClick = [this] { stopAllAudio(); };
    addAndMakeVisible(audioTestButton);
    addAndMakeVisible(panicButton);

    initialiseAudioEngine();
}

MainComponent::~MainComponent()
{
    stopAllAudio();
    soundFont.shutdown();
}

void MainComponent::initialiseAudioEngine()
{
    engineReady = soundFont.initialize();
    statusLabel.setText(engineReady
        ? "Native Windows shell ready • FluidSynth + FluidR3_GM.sf2 ready"
        : "Native Windows shell ready • Audio engine not ready",
        juce::dontSendNotification);
    audioTestButton.setEnabled(engineReady);
}

void MainComponent::playAudioTest()
{
    if (!engineReady) return;
    stopAllAudio();
    soundFont.programChange(0, 0);
    soundFont.noteOn(0, 60, 104);
    soundFont.noteOn(0, 64, 96);
    soundFont.noteOn(0, 67, 96);
    startTimer(850);
}

void MainComponent::stopAllAudio()
{
    stopTimer();
    if (engineReady) soundFont.allNotesOff();
}

void MainComponent::timerCallback()
{
    stopAllAudio();
}

void MainComponent::paint(juce::Graphics& g)
{
    g.fillAll(bg);

    auto area = getLocalBounds().reduced(28);
    area.removeFromTop(90);

    auto status = area.removeFromTop(110).toFloat();
    g.setColour(panel);
    g.fillRoundedRectangle(status, 14.0f);
    g.setColour(stroke);
    g.drawRoundedRectangle(status, 14.0f, 1.0f);

    g.setColour(engineReady ? success : accent);
    g.fillEllipse(status.getX()+22.0f, status.getY()+24.0f, 12.0f, 12.0f);

    g.setColour(text);
    g.setFont(juce::Font(juce::FontOptions(22.0f, juce::Font::bold)));
    g.drawText("NATIVE WINDOWS APPLICATION", status.toNearestInt().reduced(52,18).removeFromTop(32),
               juce::Justification::centredLeft);

    g.setColour(muted);
    g.setFont(15.0f);
    g.drawFittedText(
        "No HTML. No browser. No localhost. This shell is the permanent desktop foundation for the arranger, MIDI, style, VST3 and performance engines.",
        status.toNearestInt().reduced(52,18).withTrimmedTop(38),
        juce::Justification::topLeft, 2);

    area.removeFromTop(24);

    auto left = area.removeFromLeft((int)(area.getWidth()*0.62f));
    area.removeFromLeft(20);

    g.setColour(panel);
    g.fillRoundedRectangle(left.toFloat(), 14.0f);
    g.fillRoundedRectangle(area.toFloat(), 14.0f);

    g.setColour(text);
    g.setFont(juce::Font(juce::FontOptions(21.0f, juce::Font::bold)));
    g.drawText("ARRANGER CORE", left.reduced(22,20).removeFromTop(32), juce::Justification::centredLeft);
    g.drawText("NATIVE S1", area.reduced(22,20).removeFromTop(32), juce::Justification::centredLeft);

    g.setColour(muted);
    g.setFont(15.0f);
    g.drawFittedText("M4 timing/state engine remains the proven recovery baseline. Native UI integration now proceeds module-by-module instead of patching the old prototype.",
                     left.reduced(22,20).withTrimmedTop(44), juce::Justification::topLeft, 5);
    g.drawFittedText("This build proves the final Windows application shell and packaged audio runtime. Only working controls are exposed.",
                     area.reduced(22,20).withTrimmedTop(44), juce::Justification::topLeft, 5);
}

void MainComponent::resized()
{
    auto top = getLocalBounds().removeFromTop(90).reduced(28,12);
    auto left = top.removeFromLeft(650);

    titleLabel.setBounds(left.removeFromTop(44));
    statusLabel.setBounds(left);

    panicButton.setBounds(top.removeFromRight(220).reduced(5,10));
    audioTestButton.setBounds(top.removeFromRight(150).reduced(5,10));
}
