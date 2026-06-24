#pragma once
#include <windows.h>

class SoundFontEngine
{
public:
    bool initialize();
    void shutdown();

    void programChange(int channel, int program);
    void noteOn(int channel, int note, int velocity);
    void noteOff(int channel, int note);
    void allNotesOff();

private:
    HMODULE dll = nullptr;
    void* settings = nullptr;
    void* synth = nullptr;
    void* driver = nullptr;

    using Fn0 = void* (*)();
    using FnDel = void (*)(void*);
    using FnSetNum = int (*)(void*, const char*, double);
    using FnSetStr = int (*)(void*, const char*, const char*);
    using FnSynth = void* (*)(void*);
    using FnDelSynth = int (*)(void*);
    using FnSfLoad = int (*)(void*, const char*, int);
    using FnPC = int (*)(void*, int, int);
    using FnNoteOn = int (*)(void*, int, int, int);
    using FnNoteOff = int (*)(void*, int, int);
    using FnCC = int (*)(void*, int, int, int);
    using FnDriver = void* (*)(void*, void*);
    using FnDelDriver = void (*)(void*);

    FnDel delete_settings = nullptr;
    FnDelSynth delete_synth = nullptr;
    FnDelDriver delete_driver = nullptr;
    FnPC program_change = nullptr;
    FnNoteOn note_on = nullptr;
    FnNoteOff note_off = nullptr;
    FnCC cc = nullptr;
};
