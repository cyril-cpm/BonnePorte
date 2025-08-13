#include "Settingator.h"
#include "UARTCommunicator.h"
#include "Led.h"
#include "ledModule.h"
#include "CustomType.hpp"
#include "esp_log.h"

Settingator STR(nullptr);

extern "C" void app_main()
{
    STR.begin();

    STR.SetCommunicator(UARTCTR::CreateInstance());

    LedModule module(GPIO_NUM_4, 31);

    module.AddBiColorZone(0, 16, RGB(255, 0, 0), RGB(0, 0, 255));
    module.AddBiColorZone(16, 15, RGB(0, 0, 255), RGB(255, 0, 0));
    
     while(true)
     {
        STR.Update();
        module.Update();
     }
}