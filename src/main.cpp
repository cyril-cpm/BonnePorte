#include "Settingator.h"
#include "UARTCommunicator.h"
#include "ESPNowCommunicator.h"
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

    LedModule module(GPIO_NUM_0, 10);

    
    auto zoneVA = module.AddForeColorZone(0, 5, RGB(0, 255, 0), "VA");
    auto zoneVB = module.AddBackColorZone(5, 5, RGB(0, 255, 0), "VB");

    auto zoneB = module.AddBiColorZone(5, 5, RGB(255, 0, 0), RGB(0, 0, 255),"B");
    auto zoneA = module.AddBiColorZone(0, 5, RGB(0, 0, 255), RGB(255, 0, 0), "A");


    //auto zoneG = module.AddBiColorZone(0, 460, RGB(255, 0, 0), RGB(0, 0, 255), "ZONEGAUCHE");
    //auto zoneD = module.AddBiColorZone(460, 460, RGB(0, 0, 255), RGB(255, 0, 0), "ZONEDROITE");

    //LoadingTransition tr("LATRANSI");
    //FadingTransition fade("FADING");
    //SimplexTransition simplex("SIMPLEX");
    //simplex.AddOctave(0.09, 0.09, 1.0, "OCT");

    //LoadingTransition tr("LOADING");
    //tr.AddOctave(0.01, 0.01, 1.0, "OCT");
    //zoneG->fTransition = &simplex;
    //zoneB->fTransition = &tr;

   // zoneD->fTransition = &simplex;

   BlinkingTransition bk("BLINK");
    zoneVA->fTransition = &bk;
    zoneVB->fTransition = &bk;
    //zoneB->fTransition = &bk;
    //module.AddBiColorZone(16, 15, RGB(0, 0, 255), RGB(255, 0, 0), TRANSITION_TYPE_FADING);

    //module.AddBiColorZone(0, 31, RGB(255, 0, 0), RGB(0, 0, 255), TRANSITION_TYPE_SIMPLEX_FADE);
    
     while(true)
     {
        STR.Update();

        module.Update();

     }
}