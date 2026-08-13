#include "ChordEngine.h"
#include <algorithm>
#include <array>
#include <set>

namespace
{
struct Pattern
{
    ChordQuality quality;
    std::vector<int> intervals;
};

const std::vector<Pattern> patterns =
{
    { ChordQuality::Major7,     {0,4,7,11} },
    { ChordQuality::Minor7,     {0,3,7,10} },
    { ChordQuality::Dominant7,  {0,4,7,10} },
    { ChordQuality::Major6,     {0,4,7,9} },
    { ChordQuality::Minor6,     {0,3,7,9} },
    { ChordQuality::Add9,       {0,2,4,7} },
    { ChordQuality::Diminished, {0,3,6} },
    { ChordQuality::Augmented,  {0,4,8} },
    { ChordQuality::Sus2,       {0,2,7} },
    { ChordQuality::Sus4,       {0,5,7} },
    { ChordQuality::Minor,      {0,3,7} },
    { ChordQuality::Major,      {0,4,7} }
};
}

int ChordEngine::pitchClass(int midiNote)
{
    int pc = midiNote % 12;
    return pc < 0 ? pc + 12 : pc;
}

const char* ChordEngine::noteName(int pc)
{
    static const std::array<const char*,12> names =
    {"C","C#","D","Eb","E","F","F#","G","Ab","A","Bb","B"};

    return (pc >= 0 && pc < 12) ? names[static_cast<size_t>(pc)] : "?";
}

const char* ChordEngine::qualitySuffix(ChordQuality q)
{
    switch (q)
    {
        case ChordQuality::Major: return "";
        case ChordQuality::Minor: return "m";
        case ChordQuality::Dominant7: return "7";
        case ChordQuality::Major7: return "maj7";
        case ChordQuality::Minor7: return "m7";
        case ChordQuality::Sus2: return "sus2";
        case ChordQuality::Sus4: return "sus4";
        case ChordQuality::Diminished: return "dim";
        case ChordQuality::Augmented: return "aug";
        case ChordQuality::Major6: return "6";
        case ChordQuality::Minor6: return "m6";
        case ChordQuality::Add9: return "add9";
        default: return "?";
    }
}

std::string Chord::name() const
{
    if (!valid || root < 0) return "Unknown";

    std::string s = ChordEngine::noteName(root);
    s += ChordEngine::qualitySuffix(quality);

    if (bass >= 0 && bass != root)
    {
        s += "/";
        s += ChordEngine::noteName(bass);
    }

    return s;
}

bool ChordEngine::matchesExactly(const std::vector<int>& pitchClasses,
                                 int root,
                                 const std::vector<int>& intervals)
{
    std::set<int> expected;
    for (int interval : intervals)
        expected.insert((root + interval) % 12);

    std::set<int> actual(pitchClasses.begin(), pitchClasses.end());
    return actual == expected;
}

int ChordEngine::detectInversion(const std::vector<int>& midiNotes,
                                 int root,
                                 const std::vector<int>& intervals)
{
    if (midiNotes.empty()) return 0;

    int bassPc = pitchClass(*std::min_element(midiNotes.begin(), midiNotes.end()));

    for (size_t i = 0; i < intervals.size(); ++i)
        if (((root + intervals[i]) % 12) == bassPc)
            return static_cast<int>(i);

    return 0;
}

Chord ChordEngine::recognize(const std::vector<int>& midiNotes) const
{
    Chord result;
    if (midiNotes.size() < 3) return result;

    std::vector<int> notes = midiNotes;
    notes.erase(std::remove_if(notes.begin(), notes.end(),
        [](int n){ return n < 0 || n > 127; }), notes.end());

    if (notes.size() < 3) return result;

    std::sort(notes.begin(), notes.end());

    std::vector<int> pcs;
    for (int note : notes) pcs.push_back(pitchClass(note));

    std::sort(pcs.begin(), pcs.end());
    pcs.erase(std::unique(pcs.begin(), pcs.end()), pcs.end());

    result.bass = pitchClass(notes.front());

    for (int root = 0; root < 12; ++root)
    {
        for (const auto& p : patterns)
        {
            if (!matchesExactly(pcs, root, p.intervals)) continue;

            result.root = root;
            result.quality = p.quality;
            result.inversion = detectInversion(notes, root, p.intervals);
            result.valid = true;
            return result;
        }
    }

    return result;
}
