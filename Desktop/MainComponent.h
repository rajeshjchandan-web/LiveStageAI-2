#pragma once
#include <JuceHeader.h>
#include "Engine/SoundFontEngine.h"
#include "Engine/RealtimeArrangerEngine.h"

class MainComponent final : public juce::Component, private juce::Timer
{
public:
    MainComponent();
    ~MainComponent() override;
    void paint(juce::Graphics&) override;
    void resized() override;

private:
    struct ProLookAndFeel final : juce::LookAndFeel_V4
    {
        ProLookAndFeel();
        void drawButtonBackground(juce::Graphics&, juce::Button&, const juce::Colour&, bool, bool) override;
        void drawButtonText(juce::Graphics&, juce::TextButton&, bool, bool) override;
    };

    struct StyleItem { juce::String name, category; int bpm; };
    struct MelodyItem { juce::String name, category; int program; };

    class ListModel final : public juce::ListBoxModel
    {
    public:
        std::function<int()> count;
        std::function<void(int,juce::Graphics&,int,int,bool)> painter;
        std::function<void(int)> select;
        int getNumRows() override { return count?count():0; }
        void paintListBoxItem(int r,juce::Graphics& g,int w,int h,bool s) override { if(painter)painter(r,g,w,h,s); }
        void selectedRowsChanged(int r) override { if(select&&r>=0)select(r); }
    };

    class MixerStrip final : public juce::Component
    {
    public:
        MixerStrip(juce::String name,int channel,SoundFontEngine& sf);
        void resized() override;
    private:
        SoundFontEngine& sound;
        int channel;
        bool muted=false, soloed=false;
        double beforeMute=0.78;
        juce::Label nameLabel;
        juce::Slider fader;
        juce::TextButton solo{"S"}, mute{"M"};
    };

    void timerCallback() override;
    void setupAudio();
    void configureButton(juce::TextButton&,std::function<void()>,juce::Colour);
    void refresh();
    void updateTimer();
    void chooseStyle(int);
    void chooseMelody(int);
    void setSectionButtonStates();
    void setGlobalAccompanimentVolume(int);
    void logEvent(const juce::String&);

    ProLookAndFeel look;
    SoundFontEngine soundFont;
    RealtimeArrangerEngine arranger;
    bool engineReady=false;
    bool tempoLocked=false;
    bool recording=false;
    juce::File recordFile;

    juce::Label title,status,transport,chord,tempoValue,shiftValue;
    juce::TextButton panic{"PANIC"};

    std::vector<StyleItem> styles;
    std::vector<MelodyItem> melodies;
    int styleIndex=0, melodyIndex=0;
    ListModel styleModel, melodyModel;
    juce::ListBox styleList, melodyList;
    juce::TextEditor styleSearch, melodySearch;
    juce::Label stylesTitle,melodiesTitle;

    juce::Slider tempoKnob;
    juce::TextButton tempoDown{"- TEMPO"},tempoUp{"+ TEMPO"},tempoLock{"LOCK"},tapReset{"RESET"};
    juce::TextButton transposeDown{"TRANS -"},transposeUp{"TRANS +"},octaveDown{"OCT -"},octaveUp{"OCT +"};

    juce::TextButton syncStart{"SYNC START"},play{"PLAY"},stop{"STOP"},synchroStop{"SYNCHRO STOP"},
                     semiBar{"SEMI BAR"},fadeIn{"FADE IN"},fadeOut{"FADE OUT"},record{"RECORD"};

    juce::TextButton leftOn{"LEFT"},right1On{"RIGHT 1"},right2On{"RIGHT 2"},right3On{"RIGHT 3"};
    juce::TextButton bassMode{"BASS"},arrangerMode{"ARRANGER"},memoryMode{"MEMORY"};

    juce::TextButton introA{"INTRO A"},introB{"INTRO B"},introC{"INTRO C"};
    juce::TextButton mainA{"MAIN A"},mainB{"MAIN B"},mainC{"MAIN C"},mainD{"MAIN D"};
    juce::TextButton fillAA{"FILL AA"},fillBB{"FILL BB"},fillCC{"FILL CC"},fillDD{"FILL DD"},breakBtn{"BREAK"};
    juce::TextButton endingA{"ENDING A"},endingB{"ENDING B"},endingC{"ENDING C"};

    std::vector<std::unique_ptr<juce::TextButton>> memories;
    std::vector<std::unique_ptr<MixerStrip>> mixer;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
