#pragma once

#include "Engine/SoundFontEngine.h"
#include <string>

enum class LiveSection
{
    Stopped,
    WaitingForChord,
    IntroA,
    MainA,
    MainB,
    MainC,
    FillB,
    FillC,
    EndingA
};

class RealtimeArrangerEngine
{
public:
    explicit RealtimeArrangerEngine(SoundFontEngine& sound);

    void setTempo(double bpm);
    double getTempo() const;

    void syncStart();
    void startNow();
    void stop();

    void setChord(int rootMidi, bool minor);
    int getChordRoot() const;
    bool isMinorChord() const;
    std::string getChordName() const;

    void requestMainA();
    void requestMainB();
    void requestMainC();
    void requestFillB();
    void requestFillC();
    void requestEndingA();

    void processSixteenth();

    LiveSection getSection() const;
    std::string getSectionName() const;
    int getBar() const;
    int getBeat() const;
    bool isRunning() const;
    bool isWaitingForChord() const;

private:
    void enterSection(LiveSection next);
    void playCurrentStep();
    void playDrums(int step);
    void playBass(int step);
    void playChordTrack(int step);
    void playFill(int step, bool fillC);
    void playIntro(int step);
    void playEnding(int step);
    void applyPendingAtBoundary();
    void allTrackNotesOff();

    SoundFontEngine& sf;

    double tempoBpm = 120.0;
    LiveSection section = LiveSection::Stopped;
    LiveSection pendingSection = LiveSection::Stopped;
    bool hasPending = false;

    int step = 0;
    int bar = 1;
    int chordRoot = 60;
    bool chordMinor = false;
};
