#include "ArrangerEngine.h"
#include <chrono>
#include <iostream>
#include <thread>

ArrangerEngine::ArrangerEngine(SoundFontEngine& engine) : sf(engine)
{
    timing.setTempo(120.0);
}

void ArrangerEngine::sleepms(int ms)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

const char* ArrangerEngine::stateName() const
{
    switch (state)
    {
        case ArrangerState::Stopped: return "STOPPED";
        case ArrangerState::WaitingForChord: return "WAITING FOR CHORD";
        case ArrangerState::IntroA: return "INTRO A";
        case ArrangerState::MainA: return "MAIN A";
        case ArrangerState::MainB: return "MAIN B";
        case ArrangerState::MainC: return "MAIN C";
        case ArrangerState::FillA: return "FILL A";
        case ArrangerState::FillB: return "FILL B";
        case ArrangerState::FillC: return "FILL C";
        case ArrangerState::EndingA: return "ENDING A";
    }
    return "UNKNOWN";
}

const char* ArrangerEngine::pendingName() const
{
    switch (pending)
    {
        case PendingSection::None: return "NONE";
        case PendingSection::FillB: return "FILL B";
        case PendingSection::FillC: return "FILL C";
        case PendingSection::EndingA: return "ENDING A";
    }
    return "UNKNOWN";
}

void ArrangerEngine::setState(ArrangerState next)
{
    if (state == next) return;
    state = next;
    std::cout << "STATE -> " << stateName() << "\n";
}

std::vector<int> ArrangerEngine::voicingIntervals(ChordQuality q) const
{
    switch (q)
    {
        case ChordQuality::Minor:      return {0,3,7};
        case ChordQuality::Dominant7:  return {0,4,7,10};
        case ChordQuality::Major7:     return {0,4,7,11};
        case ChordQuality::Minor7:     return {0,3,7,10};
        case ChordQuality::Sus2:       return {0,2,7};
        case ChordQuality::Sus4:       return {0,5,7};
        case ChordQuality::Diminished: return {0,3,6};
        case ChordQuality::Augmented:  return {0,4,8};
        case ChordQuality::Major6:     return {0,4,7,9};
        case ChordQuality::Minor6:     return {0,3,7,9};
        case ChordQuality::Add9:       return {0,2,4,7};
        default:                       return {0,4,7};
    }
}

int ArrangerEngine::currentRootMidi() const
{
    return currentChord.valid ? 60 + currentChord.root : 60;
}

void ArrangerEngine::chordOn(int ch, int rootMidi, ChordQuality q, int vel)
{
    for (int interval : voicingIntervals(q))
        sf.noteOn(ch, rootMidi + interval, vel);
}

void ArrangerEngine::chordOff(int ch, int rootMidi, ChordQuality q)
{
    for (int interval : voicingIntervals(q))
        sf.noteOff(ch, rootMidi + interval);
}

void ArrangerEngine::printTransport() const
{
    std::cout << "TRANSPORT  BAR " << timing.getBar()
              << "  BEAT " << timing.getBeat()
              << "  TEMPO " << timing.getTempo()
              << " BPM  CHORD " << currentChord.name()
              << "\n";
}

void ArrangerEngine::advanceTransportAtSixteenth(int i)
{
    if ((i + 1) % 4 == 0) timing.advanceBeat();
}

void ArrangerEngine::syncStart()
{
    timing.reset();
    pending = PendingSection::None;
    currentChord = {};
    setState(ArrangerState::WaitingForChord);
    printTransport();
}

void ArrangerEngine::chordDetected(const Chord& chord)
{
    if (!chord.valid)
    {
        std::cout << "CHORD -> UNKNOWN (ignored)\n";
        return;
    }

    currentChord = chord;
    std::cout << "CHORD -> " << currentChord.name()
              << "  inversion=" << currentChord.inversion << "\n";

    if (state == ArrangerState::WaitingForChord)
    {
        setState(ArrangerState::IntroA);
        playIntroA();
        setState(ArrangerState::MainA);
    }
}

void ArrangerEngine::requestSection(PendingSection s)
{
    pending = s;
    std::cout << "REQUESTED -> " << pendingName()
              << "  (executes at next bar boundary)\n";
}

void ArrangerEngine::requestFillB(){ requestSection(PendingSection::FillB); }
void ArrangerEngine::requestFillC(){ requestSection(PendingSection::FillC); }
void ArrangerEngine::requestEndingA(){ requestSection(PendingSection::EndingA); }

bool ArrangerEngine::handlePendingAtBarBoundary()
{
    if (pending == PendingSection::None || !timing.isBarBoundary())
        return false;

    auto action = pending;
    pending = PendingSection::None;

    if (action == PendingSection::FillB)
    {
        std::cout << "BAR BOUNDARY -> EXECUTE FILL B\n";
        playFillThenMain(ArrangerState::FillB, ArrangerState::MainB);
        return true;
    }

    if (action == PendingSection::FillC)
    {
        std::cout << "BAR BOUNDARY -> EXECUTE FILL C\n";
        playFillThenMain(ArrangerState::FillC, ArrangerState::MainC);
        return true;
    }

    if (action == PendingSection::EndingA)
    {
        std::cout << "BAR BOUNDARY -> EXECUTE ENDING A\n";
        setState(ArrangerState::EndingA);
        playEndingA();
        stop();
        return true;
    }

    return false;
}

void ArrangerEngine::playIntroA()
{
    const int root = currentRootMidi();
    std::cout << "PLAY INTRO A -> MAIN A using " << currentChord.name() << "\n";

    chordOn(12, root + 12, currentChord.quality, 70);
    for (int i = 0; i < 8; ++i)
    {
        sf.noteOn(9, i % 2 ? 42 : 36, 95);
        sleepms(180);
    }
    chordOff(12, root + 12, currentChord.quality);
}

bool ArrangerEngine::playMainBar(ArrangerState mainState)
{
    setState(mainState);
    const int root = currentRootMidi();

    for (int i = 0; i < 16; ++i)
    {
        if (i % 4 == 0) printTransport();

        if (i == 0 || i == 8) sf.noteOn(9, 36, 120);
        if (i == 4 || i == 12) sf.noteOn(9, 38, 110);
        if (i % 2 == 0) sf.noteOn(9, 42, 75);

        if (i == 0 || i == 8)
        {
            sf.noteOn(10, root - 24, 112);
            sleepms(60);
            sf.noteOff(10, root - 24);
        }

        if (i == 0 || i == 4 || i == 8 || i == 12)
        {
            chordOn(11, root, currentChord.quality, 78);
            chordOn(12, root + 12, currentChord.quality, 55);
            sleepms(80);
            chordOff(11, root, currentChord.quality);
        }

        sleepms(125);
        advanceTransportAtSixteenth(i);
    }

    chordOff(12, root + 12, currentChord.quality);
    return handlePendingAtBarBoundary();
}

void ArrangerEngine::playFillThenMain(ArrangerState fillState, ArrangerState targetMain)
{
    setState(fillState);

    for (int i = 0; i < 16; ++i)
    {
        if (i % 4 == 0) printTransport();
        sf.noteOn(9, i % 2 ? 38 : 47, 118);
        if (i > 11) sf.noteOn(9, 49, 122);
        sleepms(90);
        advanceTransportAtSixteenth(i);
    }

    playMainBar(targetMain);
}

void ArrangerEngine::playEndingA()
{
    const int root = currentRootMidi();
    std::cout << "PLAY ENDING A -> STOP using " << currentChord.name() << "\n";

    chordOn(12, root + 12, currentChord.quality, 72);
    sf.noteOn(9, 49, 125);
    sleepms(450);
    sf.noteOn(9, 36, 125);
    sleepms(700);
    chordOff(12, root + 12, currentChord.quality);
}

void ArrangerEngine::stop()
{
    pending = PendingSection::None;
    sf.allNotesOff();
    setState(ArrangerState::Stopped);
}

void ArrangerEngine::playM5ProofDemo()
{
    std::cout << "\nM5 PROFESSIONAL CHORD ENGINE PROOF\n";
    std::cout << "TEMPO = " << timing.getTempo() << " BPM | 4/4\n\n";

    syncStart();

    auto cMajor = chordEngine.recognize({48,52,55});
    chordDetected(cMajor);
    playMainBar(ArrangerState::MainA);

    auto fMinor = chordEngine.recognize({53,56,60});
    chordDetected(fMinor);
    requestFillB();
    playMainBar(ArrangerState::MainA);

    auto g7 = chordEngine.recognize({55,59,62,65});
    chordDetected(g7);
    requestFillC();
    playMainBar(ArrangerState::MainB);

    auto cMaj7Inv = chordEngine.recognize({52,55,59,60});
    chordDetected(cMaj7Inv);
    requestEndingA();
    playMainBar(ArrangerState::MainC);

    std::cout << "\nM5 CHORD ENGINE PROOF COMPLETE\n";
}
