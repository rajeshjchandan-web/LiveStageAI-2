#include "SoundFontEngine.h"
#include <iostream>
#include <algorithm>

bool SoundFontEngine::initialize()
{
    dll=LoadLibraryA("libfluidsynth-3.dll");
    if(!dll){std::cout<<"FAILED: libfluidsynth-3.dll could not load. Error: "<<GetLastError()<<"\n";return false;}
    auto get=[&](const char* n){return GetProcAddress(dll,n);};
    auto new_settings=(Fn0)get("new_fluid_settings");
    delete_settings=(FnDel)get("delete_fluid_settings");
    auto set_num=(FnSetNum)get("fluid_settings_setnum");
    auto set_str=(FnSetStr)get("fluid_settings_setstr");
    auto new_synth=(FnSynth)get("new_fluid_synth");
    delete_synth=(FnDelSynth)get("delete_fluid_synth");
    auto sfload=(FnSfLoad)get("fluid_synth_sfload");
    program_change=(FnPC)get("fluid_synth_program_change");
    note_on=(FnNoteOn)get("fluid_synth_noteon");
    note_off=(FnNoteOff)get("fluid_synth_noteoff");
    cc=(FnCC)get("fluid_synth_cc");
    auto new_driver=(FnDriver)get("new_fluid_audio_driver");
    delete_driver=(FnDelDriver)get("delete_fluid_audio_driver");

    if(!new_settings||!new_synth||!sfload||!note_on||!note_off||!new_driver)return false;

    settings=new_settings();
    set_num(settings,"synth.sample-rate",44100.0);
    set_num(settings,"synth.gain",0.8);
    if(set_str)set_str(settings,"audio.driver","dsound");
    synth=new_synth(settings);

    if(sfload(synth,"SoundFonts/FluidR3_GM.sf2",1)<0)return false;

    programChange(10,33);
    programChange(11,24);
    programChange(12,48);

    driver=new_driver(settings,synth);
    if(!driver)return false;
    return true;
}

void SoundFontEngine::shutdown()
{
    allNotesOff();
    if(driver&&delete_driver)delete_driver(driver);
    if(synth&&delete_synth)delete_synth(synth);
    if(settings&&delete_settings)delete_settings(settings);
    if(dll)FreeLibrary(dll);
    driver=nullptr;synth=nullptr;settings=nullptr;dll=nullptr;
}

void SoundFontEngine::programChange(int ch,int p){if(program_change)program_change(synth,ch,p);}
void SoundFontEngine::noteOn(int ch,int n,int v){if(note_on)note_on(synth,ch,n,std::clamp(v,0,127));}
void SoundFontEngine::noteOff(int ch,int n){if(note_off)note_off(synth,ch,n);}
void SoundFontEngine::controlChange(int ch,int ctl,int val){if(cc)cc(synth,ch,ctl,std::clamp(val,0,127));}
void SoundFontEngine::setChannelVolume(int ch,int value){controlChange(ch,7,value);}
void SoundFontEngine::setChannelPan(int ch,int value){controlChange(ch,10,value);}
void SoundFontEngine::allNotesOff(){if(cc)for(int ch=0;ch<16;++ch)cc(synth,ch,123,0);}
