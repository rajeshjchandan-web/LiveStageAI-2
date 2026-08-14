#include "RealtimeArrangerEngine.h"
#include <array>
#include <algorithm>

RealtimeArrangerEngine::RealtimeArrangerEngine(SoundFontEngine& sound) : sf(sound) {}

void RealtimeArrangerEngine::setTempo(double bpm) { tempoBpm = std::clamp(bpm, 40.0, 240.0); }
double RealtimeArrangerEngine::getTempo() const { return tempoBpm; }

void RealtimeArrangerEngine::setTranspose(int semitones) { transpose = std::clamp(semitones, -12, 12); }
int RealtimeArrangerEngine::getTranspose() const { return transpose; }
void RealtimeArrangerEngine::setOctave(int octaves) { octave = std::clamp(octaves, -2, 2); }
int RealtimeArrangerEngine::getOctave() const { return octave; }

void RealtimeArrangerEngine::syncStart()
{
    allTrackNotesOff();
    step = 0; bar = 1; section = LiveSection::WaitingForChord; hasPending = false;
}

void RealtimeArrangerEngine::startNow()
{
    allTrackNotesOff();
    step = 0; bar = 1; enterSection(LiveSection::MainA);
}

void RealtimeArrangerEngine::stop()
{
    allTrackNotesOff();
    section = LiveSection::Stopped;
    hasPending = false;
    step = 0;
}

void RealtimeArrangerEngine::semiBar()
{
    if (!isRunning()) return;
    step = 8;
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
    static const std::array<const char*,12> names={"C","C#","D","Eb","E","F","F#","G","Ab","A","Bb","B"};
    int pc=(chordRoot+transpose)%12; if(pc<0)pc+=12;
    std::string s=names[(size_t)pc];
    if(chordMinor)s+="m";
    return s;
}

void RealtimeArrangerEngine::request(LiveSection next) { pendingSection=next; hasPending=true; }
void RealtimeArrangerEngine::requestIntroA(){request(LiveSection::IntroA);}
void RealtimeArrangerEngine::requestIntroB(){request(LiveSection::IntroB);}
void RealtimeArrangerEngine::requestIntroC(){request(LiveSection::IntroC);}
void RealtimeArrangerEngine::requestMainA(){request(LiveSection::MainA);}
void RealtimeArrangerEngine::requestMainB(){request(LiveSection::MainB);}
void RealtimeArrangerEngine::requestMainC(){request(LiveSection::MainC);}
void RealtimeArrangerEngine::requestMainD(){request(LiveSection::MainD);}
void RealtimeArrangerEngine::requestFillAA(){request(LiveSection::FillAA);}
void RealtimeArrangerEngine::requestFillBB(){request(LiveSection::FillBB);}
void RealtimeArrangerEngine::requestFillCC(){request(LiveSection::FillCC);}
void RealtimeArrangerEngine::requestFillDD(){request(LiveSection::FillDD);}
void RealtimeArrangerEngine::requestBreak(){request(LiveSection::Break);}
void RealtimeArrangerEngine::requestEndingA(){request(LiveSection::EndingA);}
void RealtimeArrangerEngine::requestEndingB(){request(LiveSection::EndingB);}
void RealtimeArrangerEngine::requestEndingC(){request(LiveSection::EndingC);}

void RealtimeArrangerEngine::enterSection(LiveSection next)
{
    allTrackNotesOff();
    section=next; step=0;
}

int RealtimeArrangerEngine::adjustedRoot() const { return chordRoot + transpose + octave*12; }

int RealtimeArrangerEngine::variationLevel() const
{
    switch(section)
    {
        case LiveSection::MainA: return 0;
        case LiveSection::MainB: return 1;
        case LiveSection::MainC: return 2;
        case LiveSection::MainD: return 3;
        default: return 1;
    }
}

void RealtimeArrangerEngine::processSixteenth()
{
    if(section==LiveSection::Stopped||section==LiveSection::WaitingForChord)return;
    playCurrentStep();
    ++step;

    if(step<16)return;

    step=0; ++bar;

    if(section==LiveSection::IntroA||section==LiveSection::IntroB||section==LiveSection::IntroC)
    { enterSection(LiveSection::MainA); return; }

    if(section==LiveSection::FillAA){enterSection(LiveSection::MainA);return;}
    if(section==LiveSection::FillBB){enterSection(LiveSection::MainB);return;}
    if(section==LiveSection::FillCC){enterSection(LiveSection::MainC);return;}
    if(section==LiveSection::FillDD){enterSection(LiveSection::MainD);return;}
    if(section==LiveSection::Break){enterSection(LiveSection::MainA);return;}

    if(section==LiveSection::EndingA||section==LiveSection::EndingB||section==LiveSection::EndingC)
    { stop(); return; }

    applyPendingAtBoundary();
}

void RealtimeArrangerEngine::applyPendingAtBoundary()
{
    if(!hasPending)return;
    auto next=pendingSection;
    hasPending=false;
    enterSection(next);
}

void RealtimeArrangerEngine::playCurrentStep()
{
    switch(section)
    {
        case LiveSection::IntroA: playIntro(step,0); break;
        case LiveSection::IntroB: playIntro(step,1); break;
        case LiveSection::IntroC: playIntro(step,2); break;
        case LiveSection::MainA:
        case LiveSection::MainB:
        case LiveSection::MainC:
        case LiveSection::MainD:
            playDrums(step); playBass(step); playChordTrack(step); break;
        case LiveSection::FillAA: playFill(step,0); break;
        case LiveSection::FillBB: playFill(step,1); break;
        case LiveSection::FillCC: playFill(step,2); break;
        case LiveSection::FillDD: playFill(step,3); break;
        case LiveSection::Break: playBreak(step); break;
        case LiveSection::EndingA: playEnding(step,0); break;
        case LiveSection::EndingB: playEnding(step,1); break;
        case LiveSection::EndingC: playEnding(step,2); break;
        default: break;
    }
}

void RealtimeArrangerEngine::playDrums(int s)
{
    int v=variationLevel();
    if(s==0||s==8) sf.noteOn(9,36,108+v*4);
    if(s==4||s==12) sf.noteOn(9,38,102+v*4);
    if(s%2==0) sf.noteOn(9,(v>=2&&s%4==0)?46:42,68+v*5);
    if(v>=3&&(s==6||s==14)) sf.noteOn(9,47,92);
}

void RealtimeArrangerEngine::playBass(int s)
{
    int root=adjustedRoot();
    int v=variationLevel();
    if(s==0||s==8) sf.noteOn(10,root-24,104);
    if(v>=1&&(s==4||s==12)) sf.noteOn(10,root-17,88);
    if(v>=3&&(s==6||s==14)) sf.noteOn(10,root-12,82);
}

void RealtimeArrangerEngine::playChordTrack(int s)
{
    if(s!=0&&s!=4&&s!=8&&s!=12)return;
    int root=adjustedRoot();
    int third=chordMinor?3:4;
    int vel=64+variationLevel()*7;
    sf.noteOn(11,root,vel);
    sf.noteOn(11,root+third,vel-6);
    sf.noteOn(11,root+7,vel-6);
}

void RealtimeArrangerEngine::playFill(int s,int flavour)
{
    if(s==0||s==8)sf.noteOn(9,36,116);
    int tom=45+flavour;
    if(s<8){if(s%2==0)sf.noteOn(9,tom,98+flavour*4);}
    else
    {
        sf.noteOn(9,(s%2==0)?47:50,108+flavour*3);
        if(s>=12)sf.noteOn(9,38,116);
    }
    if(s==15)sf.noteOn(9,49,124);
}

void RealtimeArrangerEngine::playIntro(int s,int flavour)
{
    int root=adjustedRoot();
    int third=chordMinor?3:4;
    if(s==0)
    {
        sf.noteOn(12,root+12,72+flavour*4);
        sf.noteOn(12,root+12+third,66+flavour*4);
        sf.noteOn(12,root+19,66+flavour*4);
        sf.noteOn(9,49,112+flavour*4);
    }
    if(s==4||s==12)sf.noteOn(9,38,92+flavour*4);
    if(s==8)sf.noteOn(9,36,108+flavour*4);
    if(s%2==0)sf.noteOn(9,42,66+flavour*3);
}

void RealtimeArrangerEngine::playEnding(int s,int flavour)
{
    int root=adjustedRoot();
    int third=chordMinor?3:4;
    if(s==0)
    {
        sf.noteOn(12,root+12,80+flavour*4);
        sf.noteOn(12,root+12+third,72+flavour*4);
        sf.noteOn(12,root+19,72+flavour*4);
        sf.noteOn(9,49,124);
    }
    if(s==4)sf.noteOn(9,38,104+flavour*4);
    if(s==8)sf.noteOn(9,36,120);
    if(s==12)sf.noteOn(9,41+flavour,108);
    if(s==15)sf.noteOn(9,49,124);
}

void RealtimeArrangerEngine::playBreak(int s)
{
    if(s==0)sf.noteOn(9,49,120);
    if(s==8)sf.noteOn(9,38,100);
    if(s==15)sf.noteOn(9,36,118);
}

void RealtimeArrangerEngine::allTrackNotesOff(){sf.allNotesOff();}

LiveSection RealtimeArrangerEngine::getSection() const{return section;}

std::string RealtimeArrangerEngine::getSectionName() const
{
    switch(section)
    {
        case LiveSection::Stopped:return "STOPPED";
        case LiveSection::WaitingForChord:return "WAITING FOR CHORD";
        case LiveSection::IntroA:return "INTRO A";
        case LiveSection::IntroB:return "INTRO B";
        case LiveSection::IntroC:return "INTRO C";
        case LiveSection::MainA:return "MAIN A";
        case LiveSection::MainB:return "MAIN B";
        case LiveSection::MainC:return "MAIN C";
        case LiveSection::MainD:return "MAIN D";
        case LiveSection::FillAA:return "FILL AA";
        case LiveSection::FillBB:return "FILL BB";
        case LiveSection::FillCC:return "FILL CC";
        case LiveSection::FillDD:return "FILL DD";
        case LiveSection::Break:return "BREAK";
        case LiveSection::EndingA:return "ENDING A";
        case LiveSection::EndingB:return "ENDING B";
        case LiveSection::EndingC:return "ENDING C";
    }
    return "UNKNOWN";
}

int RealtimeArrangerEngine::getBar() const{return bar;}
int RealtimeArrangerEngine::getBeat() const{return(step/4)+1;}
bool RealtimeArrangerEngine::isRunning() const{return section!=LiveSection::Stopped&&section!=LiveSection::WaitingForChord;}
bool RealtimeArrangerEngine::isWaitingForChord() const{return section==LiveSection::WaitingForChord;}
