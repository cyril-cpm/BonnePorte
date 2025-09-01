#include "Settingator.h"
#include "UARTCommunicator.h"
#include "Led.h"
#include "ledModule.h"
#include "CustomType.hpp"
#include "esp_log.h"
#include "Simplex.h"

Settingator STR(nullptr);

extern "C" void app_main()
{
    SIMPLEX.begin();
    STR.begin();

    STR.SetCommunicator(UARTCTR::CreateInstance());

    LedModule module(GPIO_NUM_4, 31);

    auto zone = module.AddBiColorZone(0, 16, RGB(255, 0, 0), RGB(0, 0, 255), "LAZONE");

    LoadingTransition tr("LATRANSI");
    FadingTransition fade("FADING");
    zone->fTransition = &tr;
    //module.AddBiColorZone(16, 15, RGB(0, 0, 255), RGB(255, 0, 0), TRANSITION_TYPE_FADING);

    //module.AddBiColorZone(0, 31, RGB(255, 0, 0), RGB(0, 0, 255), TRANSITION_TYPE_SIMPLEX_FADE);
    
     while(true)
     {
        STR.Update();

        module.Update();

     }
}