#pragma once

class TimingEngine
{
public:
    void setTempo(double bpm);
    void reset();

    void advanceBeat();

    int getBar() const;
    int getBeat() const;
    double getTempo() const;

    bool isBarBoundary() const;

private:
    double tempoBpm = 120.0;
    int bar = 1;
    int beat = 1;
    int beatsPerBar = 4;
};
