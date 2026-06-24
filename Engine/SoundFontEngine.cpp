#include "SoundFontEngine.h"
#include <iostream>

bool SoundFontEngine::initialize()
{
    dll = LoadLibraryA("libfluidsynth-3.dll");
    if (!dll)
    {
        std::cout << "FAILED: libfluidsynth-3.dll could not load. Error: " << GetLastError() << "\n";
        return false;
    }

    auto get = [&](const char* name) { return GetProcAddress(dll, name); };

    auto new_settings = (Fn0)get("new_fluid_settings");
    delete_settings = (FnDel)get("delete_fluid_settings");
    auto set_num = (FnSetNum)get("fluid_settings_setnum");
    auto set_str = (FnSetStr)get("fluid_settings_setstr");
    auto new_synth = (FnSynth)get("new_fluid_synth");
    delete_synth = (FnDelSynth)get("delete_fluid_synth");
    auto sfload = (FnSfLoad)get("fluid_synth_sfload");
    program_change = (FnPC)get("fluid_synth_program_change");
    note_on = (FnNoteOn)get("fluid_synth_noteon");
    note_off = (FnNoteOff)get("fluid_synth_noteoff");
    cc = (FnCC)get("fluid_synth_cc");
    auto new_driver = (FnDriver)get("new_fluid_audio_driver");
    delete_driver = (FnDelDriver)get("delete_fluid_audio_driver");

    if (!new_settings || !new_synth || !sfload || !note_on || !note_off || !new_driver)
    {
        std::cout << "FAILED: FluidSynth required functions missing.\n";
        return false;
    }

    settings = new_settings();
    set_num(settings, "synth.sample-rate", 44100.0);
    set_num(settings, "synth.gain", 0.8);
    if (set_str) set_str(settings, "audio.driver", "dsound");

    synth = new_synth(settings);

    if (sfload(synth, "SoundFonts/FluidR3_GM.sf2", 1) < 0)
    {
        std::cout << "FAILED: FluidR3_GM.sf2 could not load.\n";
        return false;
    }

    programChange(10, 33);
    programChange(11, 24);
    programChange(12, 48);

    driver = new_driver(settings, synth);
    if (!driver)
    {
        std::cout << "FAILED: FluidSynth audio driver did not start.\n";
        return false;
    }

    std::cout << "SUCCESS: FluidSynth READY\n";
    std::cout << "SUCCESS: FluidR3_GM.sf2 loaded\n";
    return true;
}

void SoundFontEngine::shutdown()
{
    allNotesOff();
    if (driver && delete_driver) delete_driver(driver);
    if (synth && delete_synth) delete_synth(synth);
    if (settings && delete_settings) delete_settings(settings);
    if (dll) FreeLibrary(dll);
}

void SoundFontEngine::programChange(int channel, int program)
{
    if (program_change) program_change(synth, channel, program);
}

void SoundFontEngine::noteOn(int channel, int note, int velocity)
{
    if (note_on) note_on(synth, channel, note, velocity);
}

void SoundFontEngine::noteOff(int channel, int note)
{
    if (note_off) note_off(synth, channel, note);
}

void SoundFontEngine::allNotesOff()
{
    if (!cc) return;
    for (int ch = 0; ch < 16; ++ch)
        cc(synth, ch, 123, 0);
}
