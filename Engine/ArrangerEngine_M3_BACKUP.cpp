#include "ArrangerEngine.h"
#include <chrono>
#include <thread>
#include <iostream>

ArrangerEngine::ArrangerEngine(SoundFontEngine& engine) : sf(engine) {}

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

void ArrangerEngine::setState(ArrangerState next)
{
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

void ArrangerEngine::syncStart()
{
    setState(ArrangerState::WaitingForChord);
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

void ArrangerEngine::playMainBar(ArrangerState mainState)
{
    setState(mainState);

    for (int i = 0; i < 16; ++i)
    {
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
    }

    chordOff(12, currentRoot + 12, currentMinor);
}

void ArrangerEngine::playFillThenMain(ArrangerState fillState, ArrangerState targetMain)
{
    setState(fillState);

    for (int i = 0; i < 16; ++i)
    {
        sf.noteOn(9, i % 2 ? 38 : 47, 118);
        if (i > 11) sf.noteOn(9, 49, 122);
        sleepms(90);
    }

    playMainBar(targetMain);
}

void ArrangerEngine::fillB()
{
    playFillThenMain(ArrangerState::FillB, ArrangerState::MainB);
}

void ArrangerEngine::fillC()
{
    playFillThenMain(ArrangerState::FillC, ArrangerState::MainC);
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

void ArrangerEngine::endingA()
{
    setState(ArrangerState::EndingA);
    playEndingA();
    stop();
}

void ArrangerEngine::stop()
{
    sf.allNotesOff();
    setState(ArrangerState::Stopped);
}

void ArrangerEngine::playM3ProofDemo()
{
    std::cout << "\nM3 ARRANGER STATE MACHINE PROOF\n";

    syncStart();

    std::cout << "SIMULATED LEFT-HAND CHORD: C major\n";
    chordDetected(60, false);

    playMainBar(ArrangerState::MainA);

    std::cout << "USER PRESSED FILL B\n";
    fillB();

    std::cout << "USER CHANGED CHORD: F minor\n";
    currentRoot = 65;
    currentMinor = true;

    playMainBar(ArrangerState::MainB);

    std::cout << "USER PRESSED FILL C\n";
    fillC();

    std::cout << "USER PRESSED ENDING A\n";
    endingA();

    std::cout << "M3 STATE MACHINE PROOF COMPLETE\n";
}
