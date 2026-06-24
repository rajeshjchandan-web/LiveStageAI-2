#pragma once
#include "Engine/SoundFontEngine.h"

class ArrangerEngine
{
public:
    explicit ArrangerEngine(SoundFontEngine& sf);
    void playProofDemo();

private:
    void sleepms(int ms);
    void chordOn(int ch, int root, bool minor, int vel);
    void chordOff(int ch, int root, bool minor);
    void mainBar(int root, bool minor);
    void fillB();

    SoundFontEngine& sf;
};
