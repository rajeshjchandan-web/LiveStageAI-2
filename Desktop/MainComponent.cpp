#include "MainComponent.h"
#include <algorithm>

namespace {
const auto BG=juce::Colour::fromRGB(7,12,20);
const auto PANEL=juce::Colour::fromRGB(17,25,38);
const auto PANEL2=juce::Colour::fromRGB(24,35,52);
const auto STROKE=juce::Colour::fromRGB(56,74,100);
const auto TEXT=juce::Colour::fromRGB(239,245,252);
const auto MUTED=juce::Colour::fromRGB(147,163,185);
const auto BLUE=juce::Colour::fromRGB(42,151,226);
const auto CYAN=juce::Colour::fromRGB(61,194,255);
const auto GREEN=juce::Colour::fromRGB(54,205,112);
const auto ORANGE=juce::Colour::fromRGB(242,143,30);
const auto PURPLE=juce::Colour::fromRGB(120,91,205);
const auto RED=juce::Colour::fromRGB(205,74,82);
const auto OLIVE=juce::Colour::fromRGB(151,134,45);
}

MainComponent::ProLookAndFeel::ProLookAndFeel()
{
    setColour(juce::TextButton::textColourOffId,TEXT);
    setColour(juce::TextButton::textColourOnId,TEXT);
}
void MainComponent::ProLookAndFeel::drawButtonBackground(juce::Graphics& g,juce::Button& b,const juce::Colour& c,bool over,bool down)
{
    auto r=b.getLocalBounds().toFloat().reduced(1);
    auto base=c.isTransparent()?PANEL2:c;
    if(over)base=base.brighter(0.12f);
    if(down)base=base.darker(0.18f);
    g.setColour(base);g.fillRoundedRectangle(r,5);
    g.setColour(STROKE.brighter(over?0.25f:0.0f));g.drawRoundedRectangle(r,5,1);
    g.setColour(juce::Colours::white.withAlpha(0.08f));g.drawLine(r.getX()+5,r.getY()+4,r.getRight()-5,r.getY()+4,1);
}
void MainComponent::ProLookAndFeel::drawButtonText(juce::Graphics& g,juce::TextButton& b,bool,bool)
{
    g.setColour(b.findColour(juce::TextButton::textColourOffId));
    g.setFont(juce::Font(juce::FontOptions(11.0f,juce::Font::bold)));
    g.drawFittedText(b.getButtonText(),b.getLocalBounds().reduced(5,2),juce::Justification::centred,2);
}

MainComponent::MixerStrip::MixerStrip(juce::String n,int ch,SoundFontEngine& sf):sound(sf),channel(ch)
{
    nameLabel.setText(n,juce::dontSendNotification);nameLabel.setJustificationType(juce::Justification::centred);
    nameLabel.setColour(juce::Label::textColourId,TEXT);nameLabel.setFont(juce::Font(juce::FontOptions(9.5f,juce::Font::bold)));
    addAndMakeVisible(nameLabel);
    fader.setSliderStyle(juce::Slider::LinearVertical);fader.setTextBoxStyle(juce::Slider::NoTextBox,false,0,0);
    fader.setRange(0,127,1);fader.setValue(100);fader.setColour(juce::Slider::trackColourId,CYAN);fader.setColour(juce::Slider::thumbColourId,juce::Colour::fromRGB(112,153,178));
    fader.onValueChange=[this]{if(!muted)sound.setChannelVolume(channel,(int)fader.getValue());};
    addAndMakeVisible(fader);
    solo.setColour(juce::TextButton::buttonColourId,PANEL2);mute.setColour(juce::TextButton::buttonColourId,PANEL2);
    solo.onClick=[this]{soloed=!soloed;solo.setToggleState(soloed,juce::dontSendNotification);};
    mute.onClick=[this]{muted=!muted;if(muted){beforeMute=fader.getValue();sound.setChannelVolume(channel,0);}else sound.setChannelVolume(channel,(int)beforeMute);mute.setToggleState(muted,juce::dontSendNotification);};
    addAndMakeVisible(solo);addAndMakeVisible(mute);
}
void MainComponent::MixerStrip::resized(){auto r=getLocalBounds().reduced(2);nameLabel.setBounds(r.removeFromTop(20));auto b=r.removeFromBottom(22);solo.setBounds(b.removeFromLeft(b.getWidth()/2).reduced(1));mute.setBounds(b.reduced(1));fader.setBounds(r.reduced(6,2));}

MainComponent::MainComponent():arranger(soundFont)
{
    setLookAndFeel(&look);setOpaque(true);
    styles={{"LoveSong","Ballad",76},{"8BeatAdria","Pop",118},{"SchlagerAlp","Schlager",98},{"SchlagerFox","Schlager",110},{"SchlagerPop","Pop",120},{"SchlagerWaltz","Waltz",90},{"SchlagerBeat","Pop",128},{"CountryPop","Country",105},{"PopGtrBallad","Ballad",70},{"MovieBallad","Ballad",72},{"SingerSongwriter","Folk",68},{"GermanRock","Rock",132},{"SwingWaltz","Swing",140},{"AnalogBallad","Ballad",64},{"80'sBoyBand","Pop",125},{"80'sMovieBallad","Ballad",74}};
    melodies={{"Concert Grand","Acoustic Piano",0},{"Studio Piano","Piano",1},{"E.Piano 1","Electric Piano",4},{"E.Piano 2","Electric Piano",5},{"Warm Pad","Pad",89},{"Choir Aahs","Choir",52},{"Strings Section","Strings",48},{"Slow Strings","Strings",49},{"Synth Lead","Lead",80},{"Soft Brass","Brass",61}};

    title.setText("LiveStage AI",juce::dontSendNotification);title.setFont(juce::Font(juce::FontOptions(27.0f,juce::Font::bold)));title.setColour(juce::Label::textColourId,TEXT);addAndMakeVisible(title);
    status.setColour(juce::Label::textColourId,MUTED);status.setFont(juce::Font(juce::FontOptions(11.0f)));addAndMakeVisible(status);
    for(auto*l:{&transport,&chord,&tempoValue,&shiftValue}){l->setColour(juce::Label::textColourId,TEXT);l->setJustificationType(juce::Justification::centred);l->setFont(juce::Font(juce::FontOptions(17.0f,juce::Font::bold)));addAndMakeVisible(*l);}

    stylesTitle.setText("STYLES",juce::dontSendNotification);stylesTitle.setColour(juce::Label::textColourId,TEXT);stylesTitle.setFont(juce::Font(juce::FontOptions(12.0f,juce::Font::bold)));addAndMakeVisible(stylesTitle);
    melodiesTitle.setText("MELODIES / VSTS",juce::dontSendNotification);melodiesTitle.setColour(juce::Label::textColourId,TEXT);melodiesTitle.setFont(juce::Font(juce::FontOptions(12.0f,juce::Font::bold)));addAndMakeVisible(melodiesTitle);
    styleSearch.setTextToShowWhenEmpty("Search styles...",MUTED);melodySearch.setTextToShowWhenEmpty("Search melodies...",MUTED);
    for(auto*e:{&styleSearch,&melodySearch}){e->setColour(juce::TextEditor::backgroundColourId,PANEL2);e->setColour(juce::TextEditor::textColourId,TEXT);addAndMakeVisible(*e);}

    styleModel.count=[this]{return(int)styles.size();};styleModel.select=[this](int i){chooseStyle(i);};
    styleModel.painter=[this](int i,juce::Graphics&g,int w,int h,bool s){if(i<0||i>=(int)styles.size())return;if(s){g.setColour(BLUE.withAlpha(.38f));g.fillRoundedRectangle(2,2,(float)w-4,(float)h-4,5);}g.setColour(TEXT);g.setFont(12.5f);g.drawText(juce::String(i+1)+".  "+styles[i].name,8,2,w-70,h/2,juce::Justification::centredLeft);g.setColour(MUTED);g.setFont(9.8f);g.drawText(styles[i].category,24,h/2-2,w-90,h/2,juce::Justification::centredLeft);g.setColour(CYAN);g.drawText(juce::String(styles[i].bpm)+" BPM",w-65,2,60,h-4,juce::Justification::centredRight);};
    styleList.setModel(&styleModel);styleList.setRowHeight(40);styleList.setColour(juce::ListBox::backgroundColourId,PANEL);addAndMakeVisible(styleList);styleList.selectRow(0);
    melodyModel.count=[this]{return(int)melodies.size();};melodyModel.select=[this](int i){chooseMelody(i);};
    melodyModel.painter=[this](int i,juce::Graphics&g,int w,int h,bool s){if(i<0||i>=(int)melodies.size())return;if(s){g.setColour(ORANGE.withAlpha(.30f));g.fillRoundedRectangle(2,2,(float)w-4,(float)h-4,5);}g.setColour(TEXT);g.setFont(12.5f);g.drawText(melodies[i].name,10,2,w-15,h/2,juce::Justification::centredLeft);g.setColour(MUTED);g.setFont(9.8f);g.drawText(melodies[i].category,10,h/2-2,w-15,h/2,juce::Justification::centredLeft);};
    melodyList.setModel(&melodyModel);melodyList.setRowHeight(40);melodyList.setColour(juce::ListBox::backgroundColourId,PANEL);addAndMakeVisible(melodyList);melodyList.selectRow(0);

    auto btn=[this](juce::TextButton&b,std::function<void()>fn,juce::Colour c=PANEL2){configureButton(b,std::move(fn),c);};
    btn(panic,[this]{arranger.stop();soundFont.allNotesOff();updateTimer();refresh();},RED);
    tempoKnob.setRange(40,240,1);tempoKnob.setValue(120);tempoKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);tempoKnob.setTextBoxStyle(juce::Slider::NoTextBox,false,0,0);tempoKnob.setColour(juce::Slider::rotarySliderFillColourId,CYAN);tempoKnob.onValueChange=[this]{if(!tempoLocked){arranger.setTempo(tempoKnob.getValue());updateTimer();refresh();}};addAndMakeVisible(tempoKnob);
    btn(tempoDown,[this]{if(!tempoLocked)tempoKnob.setValue(tempoKnob.getValue()-1);});btn(tempoUp,[this]{if(!tempoLocked)tempoKnob.setValue(tempoKnob.getValue()+1);});
    btn(tempoLock,[this]{tempoLocked=!tempoLocked;tempoLock.setToggleState(tempoLocked,juce::dontSendNotification);},ORANGE);
    btn(tapReset,[this]{tempoKnob.setValue(120);});
    btn(transposeDown,[this]{arranger.setTranspose(arranger.getTranspose()-1);refresh();});btn(transposeUp,[this]{arranger.setTranspose(arranger.getTranspose()+1);refresh();});
    btn(octaveDown,[this]{arranger.setOctave(arranger.getOctave()-1);refresh();});btn(octaveUp,[this]{arranger.setOctave(arranger.getOctave()+1);refresh();});
    btn(syncStart,[this]{arranger.syncStart();updateTimer();refresh();},BLUE);btn(play,[this]{arranger.startNow();updateTimer();refresh();},GREEN);btn(stop,[this]{arranger.stop();updateTimer();refresh();},RED);
    btn(synchroStop,[this]{arranger.requestEndingA();refresh();},PANEL2);btn(semiBar,[this]{arranger.semiBar();refresh();});
    btn(fadeIn,[this]{setGlobalAccompanimentVolume(115);});btn(fadeOut,[this]{setGlobalAccompanimentVolume(35);});
    btn(record,[this]{recording=!recording;record.setToggleState(recording,juce::dontSendNotification);if(recording){recordFile=juce::File::getSpecialLocation(juce::File::userDocumentsDirectory).getChildFile("LiveStageAI_Performance.txt");recordFile.appendText("\n--- Recording started ---\n");}else if(recordFile.existsAsFile())recordFile.appendText("--- Recording stopped ---\n");},RED);

    btn(leftOn,[this]{soundFont.setChannelVolume(4,leftOn.getToggleState()?0:100);leftOn.setToggleState(!leftOn.getToggleState(),juce::dontSendNotification);},PURPLE);
    btn(right1On,[this]{soundFont.setChannelVolume(0,right1On.getToggleState()?0:110);right1On.setToggleState(!right1On.getToggleState(),juce::dontSendNotification);},BLUE);
    btn(right2On,[this]{soundFont.setChannelVolume(1,right2On.getToggleState()?0:100);right2On.setToggleState(!right2On.getToggleState(),juce::dontSendNotification);},PURPLE);
    btn(right3On,[this]{soundFont.setChannelVolume(2,right3On.getToggleState()?0:100);right3On.setToggleState(!right3On.getToggleState(),juce::dontSendNotification);},PURPLE);
    btn(bassMode,[this]{bassMode.setToggleState(!bassMode.getToggleState(),juce::dontSendNotification);soundFont.setChannelVolume(10,bassMode.getToggleState()?105:0);},ORANGE);
    btn(arrangerMode,[this]{arrangerMode.setToggleState(!arrangerMode.getToggleState(),juce::dontSendNotification);int v=arrangerMode.getToggleState()?100:0;soundFont.setChannelVolume(11,v);soundFont.setChannelVolume(12,v);},ORANGE);
    btn(memoryMode,[this]{memoryMode.setToggleState(!memoryMode.getToggleState(),juce::dontSendNotification);},ORANGE);

    btn(introA,[this]{arranger.requestIntroA();refresh();},PURPLE);btn(introB,[this]{arranger.requestIntroB();refresh();},PURPLE);btn(introC,[this]{arranger.requestIntroC();refresh();},PURPLE);
    btn(mainA,[this]{arranger.requestMainA();refresh();},GREEN);btn(mainB,[this]{arranger.requestMainB();refresh();},GREEN);btn(mainC,[this]{arranger.requestMainC();refresh();},GREEN);btn(mainD,[this]{arranger.requestMainD();refresh();},GREEN);
    btn(fillAA,[this]{arranger.requestFillAA();refresh();},OLIVE);btn(fillBB,[this]{arranger.requestFillBB();refresh();},OLIVE);btn(fillCC,[this]{arranger.requestFillCC();refresh();},OLIVE);btn(fillDD,[this]{arranger.requestFillDD();refresh();},OLIVE);btn(breakBtn,[this]{arranger.requestBreak();refresh();},OLIVE);
    btn(endingA,[this]{arranger.requestEndingA();refresh();},RED);btn(endingB,[this]{arranger.requestEndingB();refresh();},RED);btn(endingC,[this]{arranger.requestEndingC();refresh();},RED);

    for(int i=0;i<16;++i){auto b=std::make_unique<juce::TextButton>(styles[i].name);btn(*b,[this,i]{chooseStyle(i);melodyList.selectRow(i%(int)melodies.size());},PANEL2);memories.push_back(std::move(b));}

    const juce::String mixNames[]={"RHY SUB","RHY MAIN","BASS","CHORD 1","CHORD 2","PAD","PHRASE 1","PHRASE 2","LEFT","RIGHT 1","RIGHT 2","RIGHT 3","INPUT 1","INPUT 2","INPUT 3","INPUT 4","STYLE","MELODY","PHYSICAL","MASTER"};
    const int channels[]={9,9,10,11,12,5,6,7,4,0,1,2,13,14,15,3,9,0,8,0};
    for(int i=0;i<20;++i){auto s=std::make_unique<MixerStrip>(mixNames[i],channels[i],soundFont);addAndMakeVisible(*s);mixer.push_back(std::move(s));}

    setupAudio();refresh();
}
MainComponent::~MainComponent(){stopTimer();arranger.stop();soundFont.shutdown();setLookAndFeel(nullptr);}
void MainComponent::configureButton(juce::TextButton&b,std::function<void()>fn,juce::Colour c){b.onClick=std::move(fn);b.setColour(juce::TextButton::buttonColourId,c);addAndMakeVisible(b);}
void MainComponent::setupAudio(){engineReady=soundFont.initialize();status.setText(engineReady?"AUDIO READY  •  FluidSynth  •  Functional Workstation S5":"AUDIO NOT READY",juce::dontSendNotification);}
void MainComponent::chooseStyle(int i){if(i<0||i>=(int)styles.size())return;styleIndex=i;styleList.selectRow(i);tempoKnob.setValue(styles[i].bpm);logEvent("STYLE "+styles[i].name);}
void MainComponent::chooseMelody(int i){if(i<0||i>=(int)melodies.size())return;melodyIndex=i;melodyList.selectRow(i);if(engineReady)soundFont.programChange(0,melodies[i].program);logEvent("MELODY "+melodies[i].name);}
void MainComponent::setGlobalAccompanimentVolume(int v){for(int ch:{9,10,11,12})soundFont.setChannelVolume(ch,v);}
void MainComponent::logEvent(const juce::String&s){if(recording&&recordFile!=juce::File())recordFile.appendText(juce::Time::getCurrentTime().toString(true,true)+"  "+s+"\n");}
void MainComponent::updateTimer(){if(engineReady&&arranger.isRunning())startTimer(juce::roundToInt(60000.0/arranger.getTempo()/4.0));else stopTimer();}
void MainComponent::timerCallback(){arranger.processSixteenth();if(recording&&arranger.getBeat()==1)logEvent(juce::String(arranger.getSectionName())+" / "+juce::String(arranger.getChordName()));refresh();if(!arranger.isRunning())stopTimer();}
void MainComponent::refresh(){transport.setText(juce::String(arranger.getSectionName())+"   •   BAR "+juce::String(arranger.getBar())+" / BEAT "+juce::String(arranger.getBeat()),juce::dontSendNotification);chord.setText(juce::String(arranger.getChordName()),juce::dontSendNotification);tempoValue.setText(juce::String((int)arranger.getTempo())+" BPM",juce::dontSendNotification);shiftValue.setText("TRANS "+juce::String(arranger.getTranspose())+"   •   OCT "+juce::String(arranger.getOctave()),juce::dontSendNotification);setSectionButtonStates();repaint();}
void MainComponent::setSectionButtonStates(){auto sec=arranger.getSection();mainA.setToggleState(sec==LiveSection::MainA,juce::dontSendNotification);mainB.setToggleState(sec==LiveSection::MainB,juce::dontSendNotification);mainC.setToggleState(sec==LiveSection::MainC,juce::dontSendNotification);mainD.setToggleState(sec==LiveSection::MainD,juce::dontSendNotification);}

void MainComponent::paint(juce::Graphics&g)
{
    g.fillAll(BG);auto b=getLocalBounds();auto head=b.removeFromTop(70);g.setColour(juce::Colour::fromRGB(5,10,17));g.fillRect(head);g.setColour(STROKE);g.drawLine(0,69,(float)getWidth(),69,1);
    auto left=juce::Rectangle<int>(8,80,310,getHeight()-88),right=juce::Rectangle<int>(getWidth()-318,80,310,getHeight()-88),center=juce::Rectangle<int>(326,80,getWidth()-652,getHeight()-88);
    for(auto r:{left,right}){g.setColour(PANEL);g.fillRoundedRectangle(r.toFloat(),8);g.setColour(STROKE);g.drawRoundedRectangle(r.toFloat(),8,1);}
    auto top=center.removeFromTop(210);g.setColour(PANEL);g.fillRoundedRectangle(top.toFloat(),8);center.removeFromTop(8);
    auto bank=center.removeFromTop(92);g.setColour(PANEL);g.fillRoundedRectangle(bank.toFloat(),8);center.removeFromTop(8);
    auto arranger=center.removeFromTop(86);g.setColour(PANEL);g.fillRoundedRectangle(arranger.toFloat(),8);center.removeFromTop(8);
    auto stage=center.removeFromTop(100);g.setColour(PANEL2);g.fillRoundedRectangle(stage.toFloat(),8);center.removeFromTop(8);
    g.setColour(PANEL);g.fillRoundedRectangle(center.toFloat(),8);
    g.setColour(TEXT);g.setFont(juce::Font(juce::FontOptions(12.0f,juce::Font::bold)));g.drawText("BANK MEMORIES",bank.reduced(10,4).removeFromTop(18),juce::Justification::centred);g.drawText("WORKSTATION MIXER",center.reduced(8,4).removeFromTop(18),juce::Justification::centredLeft);
    g.setColour(CYAN);g.drawText("LIVE PERFORMANCE",stage.reduced(12,6),juce::Justification::centred);
}
void MainComponent::resized()
{
    auto b=getLocalBounds();auto head=b.removeFromTop(70).reduced(16,8);title.setBounds(head.removeFromLeft(280));panic.setBounds(head.removeFromRight(100).reduced(2,6));status.setBounds(head.removeFromRight(420));
    int H=getHeight()-88;auto left=juce::Rectangle<int>(8,80,310,H).reduced(10);stylesTitle.setBounds(left.removeFromTop(24));styleSearch.setBounds(left.removeFromTop(30));left.removeFromTop(6);styleList.setBounds(left);
    auto right=juce::Rectangle<int>(getWidth()-318,80,310,H).reduced(10);melodiesTitle.setBounds(right.removeFromTop(24));melodySearch.setBounds(right.removeFromTop(30));right.removeFromTop(6);melodyList.setBounds(right);
    auto center=juce::Rectangle<int>(326,80,getWidth()-652,H);auto top=center.removeFromTop(210).reduced(8);
    auto leftTop=top.removeFromLeft(245);tempoKnob.setBounds(leftTop.removeFromLeft(120).reduced(6));auto tb=leftTop;tempoDown.setBounds(tb.removeFromTop(32).reduced(2));tempoUp.setBounds(tb.removeFromTop(32).reduced(2));tempoLock.setBounds(tb.removeFromTop(32).reduced(2));tapReset.setBounds(tb.removeFromTop(32).reduced(2));
    auto shifts=top.removeFromLeft(170);transposeDown.setBounds(shifts.removeFromTop(34).reduced(2));transposeUp.setBounds(shifts.removeFromTop(34).reduced(2));octaveDown.setBounds(shifts.removeFromTop(34).reduced(2));octaveUp.setBounds(shifts.removeFromTop(34).reduced(2));
    auto display=top.removeFromLeft(top.getWidth()-220);transport.setBounds(display.removeFromTop(50));chord.setBounds(display.removeFromTop(45));tempoValue.setBounds(display.removeFromTop(35));shiftValue.setBounds(display.removeFromTop(35));
    auto p=top;syncStart.setBounds(p.removeFromTop(32).reduced(2));play.setBounds(p.removeFromTop(32).reduced(2));stop.setBounds(p.removeFromTop(32).reduced(2));synchroStop.setBounds(p.removeFromTop(28).reduced(2));semiBar.setBounds(p.removeFromTop(28).reduced(2));auto fr=p.removeFromTop(28);fadeIn.setBounds(fr.removeFromLeft(fr.getWidth()/2).reduced(2));fadeOut.setBounds(fr.reduced(2));record.setBounds(p.removeFromTop(28).reduced(2));

    center.removeFromTop(8);auto bank=center.removeFromTop(92).reduced(8,22);int bw=bank.getWidth()/8;for(int i=0;i<16;++i)memories[i]->setBounds(bank.getX()+(i%8)*bw,bank.getY()+(i/8)*29,bw-3,26);
    center.removeFromTop(8);auto ar=center.removeFromTop(86).reduced(8,22);std::vector<juce::TextButton*> buttons={&introA,&introB,&introC,&mainA,&mainB,&mainC,&mainD,&fillAA,&fillBB,&fillCC,&fillDD,&breakBtn,&endingA,&endingB,&endingC};int uw=ar.getWidth()/(int)buttons.size();for(auto*b:buttons)b->setBounds(ar.removeFromLeft(uw).reduced(1));
    center.removeFromTop(8);auto perf=center.removeFromTop(100).reduced(8,18);std::vector<juce::TextButton*> perfButtons={&leftOn,&right1On,&right2On,&right3On,&bassMode,&arrangerMode,&memoryMode};int pw=perf.getWidth()/(int)perfButtons.size();for(auto*b:perfButtons)b->setBounds(perf.removeFromLeft(pw).reduced(3));
    center.removeFromTop(8);auto mix=center.reduced(6,22);int sw=mix.getWidth()/(int)mixer.size();for(int i=0;i<(int)mixer.size();++i)mixer[i]->setBounds(mix.getX()+i*sw,mix.getY(),sw-2,mix.getHeight());
}
