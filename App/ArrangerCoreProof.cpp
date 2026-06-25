#include "Engine/SoundFontEngine.h"
#include "Engine/ArrangerEngine.h"
#include <iostream>

int main()
{
    std::cout << "LiveStage AI 2.0 Engine M3 - Arranger State Machine\n";

    SoundFontEngine soundFont;
    if (!soundFont.initialize())
    {
        std::cout << "ENGINE FAILED\n";
        system("pause");
        return 1;
    }

    ArrangerEngine arranger(soundFont);
    arranger.playM3ProofDemo();

    soundFont.shutdown();

    std::cout << "M3 ARRANGER STATE MACHINE COMPLETE\n";
    system("pause");
    return 0;
}
