#include <JuceHeader.h>
#include "MainComponent.h"

class LiveStageAIApplication final : public juce::JUCEApplication
{
public:
    const juce::String getApplicationName() override { return "LiveStage AI"; }
    const juce::String getApplicationVersion() override { return "2.0 Native S1"; }
    bool moreThanOneInstanceAllowed() override { return false; }

    void initialise(const juce::String&) override
    {
        mainWindow = std::make_unique<MainWindow>(getApplicationName());
    }

    void shutdown() override { mainWindow.reset(); }
    void systemRequestedQuit() override { quit(); }
    void anotherInstanceStarted(const juce::String&) override {}

private:
    class MainWindow final : public juce::DocumentWindow
    {
    public:
        explicit MainWindow(juce::String name)
            : DocumentWindow(name, juce::Colour::fromRGB(13,18,29), allButtons)
        {
            setUsingNativeTitleBar(true);
            setResizable(true, true);
            setResizeLimits(1180, 720, 2560, 1600);
            setContentOwned(new MainComponent(), true);
            centreWithSize(1440, 900);
            setVisible(true);
        }

        void closeButtonPressed() override
        {
            juce::JUCEApplication::getInstance()->systemRequestedQuit();
        }
    };

    std::unique_ptr<MainWindow> mainWindow;
};

START_JUCE_APPLICATION(LiveStageAIApplication)
