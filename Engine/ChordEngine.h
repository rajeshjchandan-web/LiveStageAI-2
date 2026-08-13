#pragma once
#include <string>
#include <vector>

enum class ChordQuality
{
    Unknown, Major, Minor, Dominant7, Major7, Minor7,
    Sus2, Sus4, Diminished, Augmented, Major6, Minor6, Add9
};

struct Chord
{
    int root = -1;
    int bass = -1;
    int inversion = 0;
    ChordQuality quality = ChordQuality::Unknown;
    bool valid = false;

    std::string name() const;
};

class ChordEngine
{
public:
    Chord recognize(const std::vector<int>& midiNotes) const;

    static const char* noteName(int pitchClass);
    static const char* qualitySuffix(ChordQuality quality);

private:
    static int pitchClass(int midiNote);
    static bool matchesExactly(const std::vector<int>& pitchClasses,
                               int root,
                               const std::vector<int>& intervals);
    static int detectInversion(const std::vector<int>& midiNotes,
                               int root,
                               const std::vector<int>& chordIntervals);
};
