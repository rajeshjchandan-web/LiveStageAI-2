#include "Engine/ArrangerEngine.h"
#include "Engine/ChordEngine.h"
#include "Engine/SoundFontEngine.h"

#include <iostream>
#include <vector>

static void runRecognitionTests()
{
    ChordEngine engine;

    struct Test
    {
        const char* expected;
        std::vector<int> notes;
    };

    const std::vector<Test> tests =
    {
        { "C",     {48,52,55} },
        { "Cm",    {48,51,55} },
        { "C7",    {48,52,55,58} },
        { "Cmaj7", {48,52,55,59} },
        { "Cm7",   {48,51,55,58} },
        { "Csus4", {48,53,55} },
        { "Cdim",  {48,51,54} },
        { "Caug",  {48,52,56} },
        { "C/E",   {52,55,60} }
    };

    std::cout << "\nCHORD RECOGNITION TESTS\n";

    for (const auto& t : tests)
    {
        auto chord = engine.recognize(t.notes);
        std::cout << "Expected " << t.expected
                  << " -> Detected " << chord.name()
                  << (chord.name() == t.expected ? " [PASS]" : " [CHECK]")
                  << "\n";
    }
}

int main()
{
    std::cout << "LiveStage AI 2.0 Engine M5 - Professional Chord Engine\n";

    runRecognitionTests();

    SoundFontEngine soundFont;
    if (!soundFont.initialize())
    {
        std::cout << "ENGINE FAILED\n";
        system("pause");
        return 1;
    }

    ArrangerEngine arranger(soundFont);
    arranger.playM5ProofDemo();

    soundFont.shutdown();

    std::cout << "M5 PROFESSIONAL CHORD ENGINE COMPLETE\n";
    system("pause");
    return 0;
}
