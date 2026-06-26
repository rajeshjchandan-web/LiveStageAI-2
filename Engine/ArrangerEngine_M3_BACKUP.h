#pragma once
#include "Engine/SoundFontEngine.h"

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

class ArrangerEngine
{
public:
    explicit ArrangerEngine(SoundFontEngine& sf);

    void syncStart();
    void chordDetected(int root, bool minor);
    void fillB();
    void fillC();
    void endingA();
    void stop();

    void playM3ProofDemo();

private:
    void sleepms(int ms);
    void setState(ArrangerState next);
    const char* stateName() const;

    void chordOn(int ch, int root, bool minor, int vel);
    void chordOff(int ch, int root, bool minor);

    void playIntroA();
    void playMainBar(ArrangerState mainState);
    void playFillThenMain(ArrangerState fillState, ArrangerState targetMain);
    void playEndingA();

    SoundFontEngine& sf;
    ArrangerState state = ArrangerState::Stopped;

    int currentRoot = 60;
    bool currentMinor = false;
};
