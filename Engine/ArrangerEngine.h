#pragma once
#include "Engine/SoundFontEngine.h"
#include "Engine/TimingEngine.h"

enum class ArrangerState
{
    Stopped,
    WaitingForChord,
    IntroA,
    MainA,
    MainB,
    MainC,
    FillA,
    FillB,
    FillC,
    EndingA
};

enum class PendingSection
{
    None,
    FillB,
    FillC,
    EndingA
};

class ArrangerEngine
{
public:
    explicit ArrangerEngine(SoundFontEngine& sf);

    void syncStart();
    void chordDetected(int root, bool minor);
    void requestFillB();
    void requestFillC();
    void requestEndingA();
    void stop();

    void playM4ProofDemo();

private:
    void sleepms(int ms);
    void setState(ArrangerState next);
    const char* stateName() const;
    const char* pendingName() const;

    void chordOn(int ch, int root, bool minor, int vel);
    void chordOff(int ch, int root, bool minor);

    void printTransport() const;
    void advanceTransportAtSixteenth(int sixteenthIndex);

    void requestSection(PendingSection section);
    bool handlePendingAtBarBoundary();

    void playIntroA();
    bool playMainBar(ArrangerState mainState);
    void playFillThenMain(ArrangerState fillState, ArrangerState targetMain);
    void playEndingA();

private:
    SoundFontEngine& sf;
    TimingEngine timing;

    ArrangerState state = ArrangerState::Stopped;
    PendingSection pending = PendingSection::None;

    int currentRoot = 60;
    bool currentMinor = false;
};
