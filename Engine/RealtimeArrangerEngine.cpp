#include "RealtimeArrangerEngine.h"
#include <array>

RealtimeArrangerEngine::RealtimeArrangerEngine(SoundFontEngine& sound)
    : sf(sound)
{
}

void RealtimeArrangerEngine::setTempo(double bpm)
{
    if (bpm < 40.0) bpm = 40.0;
    if (bpm > 240.0) bpm = 240.0;
    tempoBpm = bpm;
}

double RealtimeArrangerEngine::getTempo() const { return tempoBpm; }

void RealtimeArrangerEngine::syncStart()
{
    allTrackNotesOff();
    step = 0;
    bar = 1;
    section = LiveSection::WaitingForChord;
    hasPending = false;
}

void RealtimeArrangerEngine::startNow()
{
    allTrackNotesOff();
    step = 0;
    bar = 1;
    enterSection(LiveSection::MainA);
}

void RealtimeArrangerEngine::stop()
{
    allTrackNotesOff();
    section = LiveSection::Stopped;
    hasPending = false;
    step = 0;
}

void RealtimeArrangerEngine::setChord(int rootMidi, bool minor)
{
    chordRoot = rootMidi;
    chordMinor = minor;

    if (section == LiveSection::WaitingForChord)
    {
        step = 0;
        enterSection(LiveSection::IntroA);
    }
}

int RealtimeArrangerEngine::getChordRoot() const { return chordRoot; }
bool RealtimeArrangerEngine::isMinorChord() const { return chordMinor; }

std::string RealtimeArrangerEngine::getChordName() const
{
    static const std::array<const char*, 12> names =
        {"C","C#","D","Eb","E","F","F#","G","Ab","A","Bb","B"};

    int pc = chordRoot % 12;
    if (pc < 0) pc += 12;

    std::string result = names[(size_t) pc];
    if (chordMinor) result += "m";
    return result;
}

void RealtimeArrangerEngine::requestMainA()
{
    pendingSection = LiveSection::MainA;
    hasPending = true;
}

void RealtimeArrangerEngine::requestMainB()
{
    pendingSection = LiveSection::MainB;
    hasPending = true;
}

void RealtimeArrangerEngine::requestMainC()
{
    pendingSection = LiveSection::MainC;
    hasPending = true;
}

void RealtimeArrangerEngine::requestFillB()
{
    pendingSection = LiveSection::FillB;
    hasPending = true;
}

void RealtimeArrangerEngine::requestFillC()
{
    pendingSection = LiveSection::FillC;
    hasPending = true;
}

void RealtimeArrangerEngine::requestEndingA()
{
    pendingSection = LiveSection::EndingA;
    hasPending = true;
}

void RealtimeArrangerEngine::enterSection(LiveSection next)
{
    allTrackNotesOff();
    section = next;
    step = 0;
}

void RealtimeArrangerEngine::processSixteenth()
{
    if (section == LiveSection::Stopped || section == LiveSection::WaitingForChord)
        return;

    playCurrentStep();

    ++step;

    if (step >= 16)
    {
        step = 0;
        ++bar;

        if (section == LiveSection::IntroA)
        {
            enterSection(LiveSection::MainA);
            return;
        }

        if (section == LiveSection::FillB)
        {
            enterSection(LiveSection::MainB);
            return;
        }

        if (section == LiveSection::FillC)
        {
            enterSection(LiveSection::MainC);
            return;
        }

        if (section == LiveSection::EndingA)
        {
            stop();
            return;
        }

        applyPendingAtBoundary();
    }
}

void RealtimeArrangerEngine::applyPendingAtBoundary()
{
    if (!hasPending)
        return;

    const auto requested = pendingSection;
    hasPending = false;
    enterSection(requested);
}

void RealtimeArrangerEngine::playCurrentStep()
{
    switch (section)
    {
        case LiveSection::IntroA:  playIntro(step); break;
        case LiveSection::MainA:
        case LiveSection::MainB:
        case LiveSection::MainC:
            playDrums(step);
            playBass(step);
            playChordTrack(step);
            break;
        case LiveSection::FillB:   playFill(step, false); break;
        case LiveSection::FillC:   playFill(step, true); break;
        case LiveSection::EndingA: playEnding(step); break;
        default: break;
    }
}

void RealtimeArrangerEngine::playDrums(int s)
{
    int kickVel = section == LiveSection::MainC ? 122 : 112;
    int snareVel = section == LiveSection::MainC ? 116 : 106;

    if (s == 0 || s == 8) sf.noteOn(9, 36, kickVel);
    if (s == 4 || s == 12) sf.noteOn(9, 38, snareVel);

    if (section == LiveSection::MainA)
    {
        if (s % 2 == 0) sf.noteOn(9, 42, 70);
    }
    else if (section == LiveSection::MainB)
    {
        if (s % 2 == 0) sf.noteOn(9, (s % 4 == 0) ? 46 : 42, 78);
    }
    else
    {
        sf.noteOn(9, (s % 4 == 0) ? 46 : 42, 82);
        if (s == 14) sf.noteOn(9, 47, 96);
    }
}

void RealtimeArrangerEngine::playBass(int s)
{
    if (s == 0 || s == 8)
    {
        sf.noteOn(10, chordRoot - 24, 105);
    }
    else if (section != LiveSection::MainA && (s == 4 || s == 12))
    {
        sf.noteOn(10, chordRoot - 17, 88);
    }
}

void RealtimeArrangerEngine::playChordTrack(int s)
{
    if (s != 0 && s != 4 && s != 8 && s != 12)
        return;

    const int third = chordMinor ? 3 : 4;
    const int velocity = section == LiveSection::MainA ? 64 :
                         section == LiveSection::MainB ? 76 : 86;

    sf.noteOn(11, chordRoot, velocity);
    sf.noteOn(11, chordRoot + third, velocity - 6);
    sf.noteOn(11, chordRoot + 7, velocity - 6);
}

void RealtimeArrangerEngine::playFill(int s, bool fillC)
{
    if (s == 0 || s == 8) sf.noteOn(9, 36, 118);

    if (s < 8)
    {
        if (s % 2 == 0) sf.noteOn(9, fillC ? 45 : 47, 104);
    }
    else
    {
        sf.noteOn(9, (s % 2 == 0) ? 47 : 50, 112);
        if (s >= 12) sf.noteOn(9, 38, 118);
    }

    if (s == 15) sf.noteOn(9, 49, 125);
}

void RealtimeArrangerEngine::playIntro(int s)
{
    if (s == 0)
    {
        const int third = chordMinor ? 3 : 4;
        sf.noteOn(12, chordRoot + 12, 74);
        sf.noteOn(12, chordRoot + 12 + third, 66);
        sf.noteOn(12, chordRoot + 19, 66);
        sf.noteOn(9, 49, 116);
    }

    if (s == 4 || s == 12) sf.noteOn(9, 38, 94);
    if (s == 8) sf.noteOn(9, 36, 112);
    if (s % 2 == 0) sf.noteOn(9, 42, 68);
}

void RealtimeArrangerEngine::playEnding(int s)
{
    if (s == 0)
    {
        const int third = chordMinor ? 3 : 4;
        sf.noteOn(12, chordRoot + 12, 82);
        sf.noteOn(12, chordRoot + 12 + third, 74);
        sf.noteOn(12, chordRoot + 19, 74);
        sf.noteOn(9, 49, 125);
    }

    if (s == 4) sf.noteOn(9, 38, 108);
    if (s == 8) sf.noteOn(9, 36, 125);
    if (s == 12) sf.noteOn(9, 41, 112);
    if (s == 15) sf.noteOn(9, 49, 125);
}

void RealtimeArrangerEngine::allTrackNotesOff()
{
    sf.allNotesOff();
}

LiveSection RealtimeArrangerEngine::getSection() const { return section; }

std::string RealtimeArrangerEngine::getSectionName() const
{
    switch (section)
    {
        case LiveSection::Stopped: return "STOPPED";
        case LiveSection::WaitingForChord: return "WAITING FOR CHORD";
        case LiveSection::IntroA: return "INTRO A";
        case LiveSection::MainA: return "MAIN A";
        case LiveSection::MainB: return "MAIN B";
        case LiveSection::MainC: return "MAIN C";
        case LiveSection::FillB: return "FILL B";
        case LiveSection::FillC: return "FILL C";
        case LiveSection::EndingA: return "ENDING A";
    }
    return "UNKNOWN";
}

int RealtimeArrangerEngine::getBar() const { return bar; }
int RealtimeArrangerEngine::getBeat() const { return (step / 4) + 1; }

bool RealtimeArrangerEngine::isRunning() const
{
    return section != LiveSection::Stopped && section != LiveSection::WaitingForChord;
}

bool RealtimeArrangerEngine::isWaitingForChord() const
{
    return section == LiveSection::WaitingForChord;
}
