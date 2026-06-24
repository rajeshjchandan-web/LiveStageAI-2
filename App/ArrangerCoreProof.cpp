#include "Engine/SoundFontEngine.h"
#include "Engine/ArrangerEngine.h"
#include <iostream>

int main()
{
    std::cout << "LiveStage AI 2.0 Engine M2 - Modular Arranger Proof\n";

    SoundFontEngine soundFont;
    if (!soundFont.initialize())
    {
        std::cout << "ENGINE FAILED\n";
        system("pause");
        return 1;
    }

    ArrangerEngine arranger(soundFont);
    arranger.playProofDemo();

    soundFont.shutdown();

    std::cout << "M2 MODULAR ENGINE PROOF COMPLETE\n";
    system("pause");
    return 0;
}
