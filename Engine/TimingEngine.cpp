#include "TimingEngine.h"

void TimingEngine::setTempo(double bpm)
{
    tempoBpm = bpm;
}

void TimingEngine::reset()
{
    bar = 1;
    beat = 1;
}

void TimingEngine::advanceBeat()
{
    beat++;

    if (beat > beatsPerBar)
    {
        beat = 1;
        bar++;
    }
}

int TimingEngine::getBar() const
{
    return bar;
}

int TimingEngine::getBeat() const
{
    return beat;
}

double TimingEngine::getTempo() const
{
    return tempoBpm;
}

bool TimingEngine::isBarBoundary() const
{
    return beat == 1;
}
