#pragma once
#include "Engine/ChordEngine.h"
#include "Engine/SoundFontEngine.h"
#include "Engine/TimingEngine.h"

enum class ArrangerState
{
    Stopped, WaitingForChord, IntroA,
    MainA, MainB, MainC,
    FillA, FillB, FillC,
    EndingA
};

enum class PendingSection
{
    None, FillB, FillC, EndingA
};

class ArrangerEngine
{
public:
    explicit ArrangerEngine(SoundFontEngine& sf);

    void syncStart();
    void chordDetected(const Chord& chord);
    void requestFillB();
    void requestFillC();
    void requestEndingA();
    void stop();

    void playM5ProofDemo();

private:
    void sleepms(int ms);
    void setState(ArrangerState next);
    const char* stateName() const;
    const char* pendingName() const;

    void chordOn(int ch, int rootMidi, ChordQuality quality, int vel);
    void chordOff(int ch, int rootMidi, ChordQuality quality);

    void printTransport() const;
    void advanceTransportAtSixteenth(int sixteenthIndex);
    void requestSection(PendingSection section);
    bool handlePendingAtBarBoundary();

    void playIntroA();
    bool playMainBar(ArrangerState mainState);
    void playFillThenMain(ArrangerState fillState, ArrangerState targetMain);
    void playEndingA();

    std::vector<int> voicingIntervals(ChordQuality quality) const;
    int currentRootMidi() const;

    SoundFontEngine& sf;
    TimingEngine timing;
    ChordEngine chordEngine;

    ArrangerState state = ArrangerState::Stopped;
    PendingSection pending = PendingSection::None;
    Chord currentChord;
};
