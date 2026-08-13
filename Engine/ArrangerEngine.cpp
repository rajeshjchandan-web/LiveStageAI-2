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

void ArrangerEngine::chordOn(int ch, int root, bool minor, int vel)
{
    sf.noteOn(ch, root, vel);
    sf.noteOn(ch, root + (minor ? 3 : 4), vel - 8);
    sf.noteOn(ch, root + 7, vel - 8);
}

void ArrangerEngine::chordOff(int ch, int root, bool minor)
{
    sf.noteOff(ch, root);
    sf.noteOff(ch, root + (minor ? 3 : 4));
    sf.noteOff(ch, root + 7);
}

void ArrangerEngine::printTransport() const
{
    std::cout << "TRANSPORT  BAR " << timing.getBar()
              << "  BEAT " << timing.getBeat()
              << "  TEMPO " << timing.getTempo() << " BPM\n";
}

void ArrangerEngine::advanceTransportAtSixteenth(int sixteenthIndex)
{
    if ((sixteenthIndex + 1) % 4 == 0)
        timing.advanceBeat();
}

void ArrangerEngine::syncStart()
{
    timing.reset();
    pending = PendingSection::None;
    setState(ArrangerState::WaitingForChord);
    printTransport();
}

void ArrangerEngine::chordDetected(int root, bool minor)
{
    currentRoot = root;
    currentMinor = minor;

    if (state == ArrangerState::WaitingForChord)
    {
        setState(ArrangerState::IntroA);
        playIntroA();
        setState(ArrangerState::MainA);
    }
}

void ArrangerEngine::requestSection(PendingSection section)
{
    pending = section;
    std::cout << "REQUESTED -> " << pendingName()
              << "  (executes at next bar boundary)\n";
}

void ArrangerEngine::requestFillB() { requestSection(PendingSection::FillB); }
void ArrangerEngine::requestFillC() { requestSection(PendingSection::FillC); }
void ArrangerEngine::requestEndingA() { requestSection(PendingSection::EndingA); }

bool ArrangerEngine::handlePendingAtBarBoundary()
{
    if (pending == PendingSection::None || !timing.isBarBoundary())
        return false;

    auto action = pending;
    pending = PendingSection::None;

    switch (action)
    {
        case PendingSection::FillB:
            std::cout << "BAR BOUNDARY -> EXECUTE FILL B\n";
            playFillThenMain(ArrangerState::FillB, ArrangerState::MainB);
            return true;
        case PendingSection::FillC:
            std::cout << "BAR BOUNDARY -> EXECUTE FILL C\n";
            playFillThenMain(ArrangerState::FillC, ArrangerState::MainC);
            return true;
        case PendingSection::EndingA:
            std::cout << "BAR BOUNDARY -> EXECUTE ENDING A\n";
            setState(ArrangerState::EndingA);
            playEndingA();
            stop();
            return true;
        case PendingSection::None:
            break;
    }
    return false;
}

void ArrangerEngine::playIntroA()
{
    std::cout << "PLAY INTRO A -> MAIN A\n";
    chordOn(12, currentRoot + 12, currentMinor, 70);

    for (int i = 0; i < 8; ++i)
    {
        sf.noteOn(9, i % 2 ? 42 : 36, 95);
        sleepms(180);
    }

    chordOff(12, currentRoot + 12, currentMinor);
}

bool ArrangerEngine::playMainBar(ArrangerState mainState)
{
    setState(mainState);

    for (int i = 0; i < 16; ++i)
    {
        if (i % 4 == 0) printTransport();

        if (i == 0 || i == 8) sf.noteOn(9, 36, 120);
        if (i == 4 || i == 12) sf.noteOn(9, 38, 110);
        if (i % 2 == 0) sf.noteOn(9, 42, 75);

        if (i == 0 || i == 8)
        {
            sf.noteOn(10, currentRoot - 24, 112);
            sleepms(60);
            sf.noteOff(10, currentRoot - 24);
        }

        if (i == 0 || i == 4 || i == 8 || i == 12)
        {
            chordOn(11, currentRoot, currentMinor, 78);
            chordOn(12, currentRoot + 12, currentMinor, 55);
            sleepms(80);
            chordOff(11, currentRoot, currentMinor);
        }

        sleepms(125);
        advanceTransportAtSixteenth(i);
    }

    chordOff(12, currentRoot + 12, currentMinor);
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
    std::cout << "PLAY ENDING A -> STOP\n";
    chordOn(12, currentRoot + 12, currentMinor, 72);
    sf.noteOn(9, 49, 125);
    sleepms(450);
    sf.noteOn(9, 36, 125);
    sleepms(700);
    chordOff(12, currentRoot + 12, currentMinor);
}

void ArrangerEngine::stop()
{
    pending = PendingSection::None;
    sf.allNotesOff();
    setState(ArrangerState::Stopped);
}

void ArrangerEngine::playM4ProofDemo()
{
    std::cout << "\nM4 MUSICAL TIMING ENGINE PROOF\n";
    std::cout << "TEMPO = " << timing.getTempo() << " BPM | 4/4\n\n";

    syncStart();

    std::cout << "SIMULATED LEFT-HAND CHORD: C major\n";
    chordDetected(60, false);

    std::cout << "\nPLAY MAIN A\n";
    playMainBar(ArrangerState::MainA);

    std::cout << "\nREQUEST FILL B DURING MAIN A\n";
    requestFillB();
    playMainBar(ArrangerState::MainA);

    std::cout << "\nCHORD CHANGE: F minor\n";
    currentRoot = 65;
    currentMinor = true;

    std::cout << "\nREQUEST FILL C DURING MAIN B\n";
    requestFillC();
    playMainBar(ArrangerState::MainB);

    std::cout << "\nREQUEST ENDING A DURING MAIN C\n";
    requestEndingA();
    playMainBar(ArrangerState::MainC);

    std::cout << "\nM4 TIMING ENGINE PROOF COMPLETE\n";
}
