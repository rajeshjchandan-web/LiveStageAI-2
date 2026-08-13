#include "MainComponent.h"

namespace
{
const auto bg       = juce::Colour::fromRGB(8, 13, 22);
const auto panel    = juce::Colour::fromRGB(18, 25, 38);
const auto panel2   = juce::Colour::fromRGB(24, 33, 49);
const auto stroke   = juce::Colour::fromRGB(52, 67, 91);
const auto accent   = juce::Colour::fromRGB(61, 194, 255);
const auto green    = juce::Colour::fromRGB(59, 214, 126);
const auto amber    = juce::Colour::fromRGB(255, 187, 78);
const auto red      = juce::Colour::fromRGB(255, 92, 104);
const auto text     = juce::Colour::fromRGB(239, 244, 251);
const auto muted    = juce::Colour::fromRGB(151, 164, 184);

void styleReadout(juce::Label& label, float size)
{
    label.setColour(juce::Label::textColourId, text);
    label.setFont(juce::Font(juce::FontOptions(size, juce::Font::bold)));
    label.setJustificationType(juce::Justification::centred);
}
}

MainComponent::MainComponent()
    : arranger(soundFont)
{
    setOpaque(true);

    titleLabel.setText("LiveStage AI", juce::dontSendNotification);
    titleLabel.setFont(juce::Font(juce::FontOptions(30.0f, juce::Font::bold)));
    titleLabel.setColour(juce::Label::textColourId, text);
    addAndMakeVisible(titleLabel);

    engineStatusLabel.setColour(juce::Label::textColourId, muted);
    engineStatusLabel.setFont(juce::Font(juce::FontOptions(13.0f)));
    addAndMakeVisible(engineStatusLabel);

    styleReadout(sectionValue, 24.0f);
    styleReadout(chordValue, 30.0f);
    styleReadout(barBeatValue, 22.0f);
    styleReadout(tempoValue, 22.0f);

    addAndMakeVisible(sectionValue);
    addAndMakeVisible(chordValue);
    addAndMakeVisible(barBeatValue);
    addAndMakeVisible(tempoValue);

    tempoSlider.setRange(40.0, 240.0, 1.0);
    tempoSlider.setValue(120.0);
    tempoSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    tempoSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 74, 28);
    tempoSlider.setColour(juce::Slider::trackColourId, accent);
    tempoSlider.setColour(juce::Slider::thumbColourId, text);
    tempoSlider.onValueChange = [this]
    {
        arranger.setTempo(tempoSlider.getValue());
        updateTransportTimer();
        refreshReadouts();
    };
    addAndMakeVisible(tempoSlider);

    const char* names[] = {"C","C#","D","Eb","E","F","F#","G","Ab","A","Bb","B"};
    for (int i = 0; i < 12; ++i)
        chordRootBox.addItem(names[i], i + 1);

    chordRootBox.setSelectedId(1);
    chordRootBox.onChange = [this]
    {
        const int root = 60 + (chordRootBox.getSelectedId() - 1);
        arranger.setChord(root, minorToggle.getToggleState());
        refreshReadouts();
    };
    addAndMakeVisible(chordRootBox);

    minorToggle.onClick = [this]
    {
        const int root = 60 + (chordRootBox.getSelectedId() - 1);
        arranger.setChord(root, minorToggle.getToggleState());
        refreshReadouts();
    };
    addAndMakeVisible(minorToggle);

    configureButton(syncStartButton, [this]
    {
        if (!engineReady) return;
        arranger.syncStart();
        updateTransportTimer();
        refreshReadouts();
    }, true);

    configureButton(startButton, [this]
    {
        if (!engineReady) return;
        arranger.startNow();
        updateTransportTimer();
        refreshReadouts();
    }, true);

    configureButton(stopButton, [this]
    {
        arranger.stop();
        updateTransportTimer();
        refreshReadouts();
    });

    configureButton(mainAButton, [this] { arranger.requestMainA(); refreshReadouts(); });
    configureButton(mainBButton, [this] { arranger.requestMainB(); refreshReadouts(); });
    configureButton(mainCButton, [this] { arranger.requestMainC(); refreshReadouts(); });
    configureButton(fillBButton, [this] { arranger.requestFillB(); refreshReadouts(); }, true);
    configureButton(fillCButton, [this] { arranger.requestFillC(); refreshReadouts(); }, true);
    configureButton(endingButton, [this] { arranger.requestEndingA(); refreshReadouts(); });
    configureButton(panicButton, [this] { stopEverything(); });

    initialiseAudioEngine();
    refreshReadouts();
}

MainComponent::~MainComponent()
{
    stopTimer();
    arranger.stop();
    soundFont.shutdown();
}

void MainComponent::configureButton(juce::TextButton& button,
                                    std::function<void()> callback,
                                    bool isAccent)
{
    button.onClick = std::move(callback);
    button.setColour(juce::TextButton::buttonColourId,
                     isAccent ? accent.withAlpha(0.32f) : panel2);
    button.setColour(juce::TextButton::buttonOnColourId, accent);
    button.setColour(juce::TextButton::textColourOffId, text);
    addAndMakeVisible(button);
}

void MainComponent::initialiseAudioEngine()
{
    engineReady = soundFont.initialize();

    engineStatusLabel.setText(
        engineReady
            ? "AUDIO READY  •  FluidSynth + FluidR3_GM.sf2  •  Native realtime arranger S2"
            : "AUDIO NOT READY  •  Check packaged runtime files",
        juce::dontSendNotification);

    for (auto* b : { &syncStartButton, &startButton, &mainAButton, &mainBButton,
                     &mainCButton, &fillBButton, &fillCButton, &endingButton })
        b->setEnabled(engineReady);
}

void MainComponent::updateTransportTimer()
{
    if (!engineReady)
    {
        stopTimer();
        return;
    }

    if (arranger.isRunning())
    {
        const double sixteenthMs = 60000.0 / arranger.getTempo() / 4.0;
        startTimer(juce::roundToInt(sixteenthMs));
    }
    else
    {
        stopTimer();
    }
}

void MainComponent::timerCallback()
{
    arranger.processSixteenth();
    refreshReadouts();

    if (!arranger.isRunning())
        stopTimer();
}

void MainComponent::refreshReadouts()
{
    sectionValue.setText(juce::String(arranger.getSectionName()), juce::dontSendNotification);
    chordValue.setText(juce::String(arranger.getChordName()), juce::dontSendNotification);

    barBeatValue.setText(
        "BAR " + juce::String(arranger.getBar()) +
        "  •  BEAT " + juce::String(arranger.getBeat()),
        juce::dontSendNotification);

    tempoValue.setText(
        juce::String((int) arranger.getTempo()) + " BPM",
        juce::dontSendNotification);

    repaint();
}

void MainComponent::stopEverything()
{
    arranger.stop();
    if (engineReady) soundFont.allNotesOff();
    stopTimer();
    refreshReadouts();
}

void MainComponent::paint(juce::Graphics& g)
{
    g.fillAll(bg);

    auto bounds = getLocalBounds();

    auto header = bounds.removeFromTop(92);
    g.setColour(panel);
    g.fillRect(header);
    g.setColour(stroke);
    g.drawLine(0.0f, 91.0f, (float) getWidth(), 91.0f, 1.0f);

    auto area = bounds.reduced(28);

    auto display = area.removeFromTop(155);
    g.setColour(panel);
    g.fillRoundedRectangle(display.toFloat(), 14.0f);
    g.setColour(stroke);
    g.drawRoundedRectangle(display.toFloat(), 14.0f, 1.0f);

    auto displayInner = display.reduced(18);
    const int col = displayInner.getWidth() / 4;

    auto sectionBox = displayInner.removeFromLeft(col);
    auto chordBox = displayInner.removeFromLeft(col);
    auto barBox = displayInner.removeFromLeft(col);
    auto tempoBox = displayInner;

    auto drawCaption = [&g](juce::Rectangle<int> r, const juce::String& caption, juce::Colour c)
    {
        g.setColour(c);
        g.setFont(juce::Font(juce::FontOptions(12.0f, juce::Font::bold)));
        g.drawText(caption, r.removeFromTop(24), juce::Justification::centred);
    };

    drawCaption(sectionBox, "SECTION", accent);
    drawCaption(chordBox, "CHORD", green);
    drawCaption(barBox, "TRANSPORT", amber);
    drawCaption(tempoBox, "TEMPO", accent);

    area.removeFromTop(22);

    auto performance = area.removeFromTop(220);
    g.setColour(panel);
    g.fillRoundedRectangle(performance.toFloat(), 14.0f);
    g.setColour(stroke);
    g.drawRoundedRectangle(performance.toFloat(), 14.0f, 1.0f);

    g.setColour(text);
    g.setFont(juce::Font(juce::FontOptions(18.0f, juce::Font::bold)));
    g.drawText("LIVE ARRANGER", performance.reduced(20, 14).removeFromTop(30),
               juce::Justification::centredLeft);

    area.removeFromTop(22);

    g.setColour(panel2);
    g.fillRoundedRectangle(area.toFloat(), 14.0f);
    g.setColour(stroke);
    g.drawRoundedRectangle(area.toFloat(), 14.0f, 1.0f);

    g.setColour(text);
    g.setFont(juce::Font(juce::FontOptions(17.0f, juce::Font::bold)));
    g.drawText("S2 REALTIME ENGINE", area.reduced(20, 16).removeFromTop(28),
               juce::Justification::centredLeft);

    g.setColour(muted);
    g.setFont(14.0f);
    g.drawFittedText(
        "The GUI stays responsive while the native timer drives 16th-note accompaniment. "
        "Main and fill requests quantize to the next bar. Sync Start waits for a chord selection. "
        "Ending plays one bar and stops automatically.",
        area.reduced(20, 16).withTrimmedTop(38),
        juce::Justification::topLeft, 4);
}

void MainComponent::resized()
{
    auto bounds = getLocalBounds();

    auto header = bounds.removeFromTop(92).reduced(28, 12);
    auto leftHeader = header.removeFromLeft(720);

    titleLabel.setBounds(leftHeader.removeFromTop(43));
    engineStatusLabel.setBounds(leftHeader);

    panicButton.setBounds(header.removeFromRight(120).reduced(4, 10));

    auto area = bounds.reduced(28);

    auto display = area.removeFromTop(155).reduced(18);
    const int col = display.getWidth() / 4;

    auto sectionBox = display.removeFromLeft(col);
    auto chordBox = display.removeFromLeft(col);
    auto barBox = display.removeFromLeft(col);
    auto tempoBox = display;

    sectionBox.removeFromTop(26);
    chordBox.removeFromTop(26);
    barBox.removeFromTop(26);
    tempoBox.removeFromTop(26);

    sectionValue.setBounds(sectionBox);
    chordValue.setBounds(chordBox);
    barBeatValue.setBounds(barBox);
    tempoValue.setBounds(tempoBox);

    area.removeFromTop(22);
    auto performance = area.removeFromTop(220).reduced(20, 52);

    auto firstRow = performance.removeFromTop(58);
    auto buttonW = (firstRow.getWidth() - 36) / 4;

    syncStartButton.setBounds(firstRow.removeFromLeft(buttonW).reduced(3));
    firstRow.removeFromLeft(12);
    startButton.setBounds(firstRow.removeFromLeft(buttonW).reduced(3));
    firstRow.removeFromLeft(12);
    stopButton.setBounds(firstRow.removeFromLeft(buttonW).reduced(3));
    firstRow.removeFromLeft(12);
    endingButton.setBounds(firstRow.reduced(3));

    performance.removeFromTop(12);
    auto secondRow = performance.removeFromTop(58);
    buttonW = (secondRow.getWidth() - 48) / 5;

    mainAButton.setBounds(secondRow.removeFromLeft(buttonW).reduced(3));
    secondRow.removeFromLeft(12);
    mainBButton.setBounds(secondRow.removeFromLeft(buttonW).reduced(3));
    secondRow.removeFromLeft(12);
    mainCButton.setBounds(secondRow.removeFromLeft(buttonW).reduced(3));
    secondRow.removeFromLeft(12);
    fillBButton.setBounds(secondRow.removeFromLeft(buttonW).reduced(3));
    secondRow.removeFromLeft(12);
    fillCButton.setBounds(secondRow.reduced(3));

    area.removeFromTop(22);
    auto controls = area.reduced(20, 70);

    auto chordControls = controls.removeFromLeft(310);
    chordRootBox.setBounds(chordControls.removeFromLeft(120).reduced(3, 7));
    minorToggle.setBounds(chordControls.removeFromLeft(95).reduced(8, 7));

    controls.removeFromLeft(30);
    tempoSlider.setBounds(controls.removeFromLeft(480).reduced(3, 7));
}
