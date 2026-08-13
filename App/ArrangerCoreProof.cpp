#include "Engine/SoundFontEngine.h"
#include "Engine/ArrangerEngine.h"
#include <iostream>

int main()
{
    std::cout << "LiveStage AI 2.0 Engine M4 - Musical Timing Engine\n";

    SoundFontEngine soundFont;
    if (!soundFont.initialize())
    {
        std::cout << "ENGINE FAILED\n";
        system("pause");
        return 1;
    }

    ArrangerEngine arranger(soundFont);
    arranger.playM4ProofDemo();

    soundFont.shutdown();

    std::cout << "M4 MUSICAL TIMING ENGINE COMPLETE\n";
    system("pause");
    return 0;
}
