#pragma once
#include "Engine/SoundFontEngine.h"
#include <string>

enum class LiveSection
{
    Stopped, WaitingForChord,
    IntroA, IntroB, IntroC,
    MainA, MainB, MainC, MainD,
    FillAA, FillBB, FillCC, FillDD, Break,
    EndingA, EndingB, EndingC
};

class RealtimeArrangerEngine
{
public:
    explicit RealtimeArrangerEngine(SoundFontEngine& sound);

    void setTempo(double bpm);
    double getTempo() const;

    void setTranspose(int semitones);
    int getTranspose() const;
    void setOctave(int octaves);
    int getOctave() const;

    void syncStart();
    void startNow();
    void stop();
    void semiBar();

    void setChord(int rootMidi, bool minor);
    int getChordRoot() const;
    bool isMinorChord() const;
    std::string getChordName() const;

    void requestIntroA();
    void requestIntroB();
    void requestIntroC();
    void requestMainA();
    void requestMainB();
    void requestMainC();
    void requestMainD();
    void requestFillAA();
    void requestFillBB();
    void requestFillCC();
    void requestFillDD();
    void requestBreak();
    void requestEndingA();
    void requestEndingB();
    void requestEndingC();

    void processSixteenth();

    LiveSection getSection() const;
    std::string getSectionName() const;
    int getBar() const;
    int getBeat() const;
    bool isRunning() const;
    bool isWaitingForChord() const;

private:
    void request(LiveSection next);
    void enterSection(LiveSection next);
    void playCurrentStep();
    void playDrums(int step);
    void playBass(int step);
    void playChordTrack(int step);
    void playFill(int step, int flavour);
    void playIntro(int step, int flavour);
    void playEnding(int step, int flavour);
    void playBreak(int step);
    void applyPendingAtBoundary();
    void allTrackNotesOff();

    int adjustedRoot() const;
    int variationLevel() const;

    SoundFontEngine& sf;

    double tempoBpm = 120.0;
    LiveSection section = LiveSection::Stopped;
    LiveSection pendingSection = LiveSection::Stopped;
    bool hasPending = false;

    int step = 0;
    int bar = 1;
    int chordRoot = 60;
    bool chordMinor = false;
    int transpose = 0;
    int octave = 0;
};
