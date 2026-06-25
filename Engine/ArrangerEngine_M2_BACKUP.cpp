#include "ArrangerEngine.h"
#include <chrono>
#include <thread>
#include <iostream>

ArrangerEngine::ArrangerEngine(SoundFontEngine& engine) : sf(engine) {}

void ArrangerEngine::sleepms(int ms)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
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

void ArrangerEngine::mainBar(int root, bool minor)
{
    for (int i = 0; i < 16; ++i)
    {
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
            chordOn(11, root, minor, 78);
            chordOn(12, root + 12, minor, 55);
            sleepms(80);
            chordOff(11, root, minor);
        }

        sleepms(125);
    }

    chordOff(12, root + 12, minor);
}

void ArrangerEngine::fillB()
{
    std::cout << "FILL B\n";
    for (int i = 0; i < 16; ++i)
    {
        sf.noteOn(9, i % 2 ? 38 : 47, 118);
        sleepms(90);
    }
}

void ArrangerEngine::playProofDemo()
{
    std::cout << "MAIN A C major\n";
    mainBar(60, false);

    fillB();

    std::cout << "MAIN B F minor\n";
    mainBar(65, true);

    std::cout << "ENDING\n";
    sf.noteOn(9, 49, 120);
    sleepms(500);
    sf.noteOn(9, 36, 120);
    sleepms(700);

    sf.allNotesOff();
}
