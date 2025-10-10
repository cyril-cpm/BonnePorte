#include "Settingator.h"
#include "UARTCommunicator.h"
#include "ESPNowCommunicator.h"
#include "Led.h"
#include "ledModule.h"
#include "CustomType.hpp"
#include "esp_log.h"
#include "Simplex.h"

Settingator STR(nullptr);

#define ALL_LED_LEN  920
#define ZONE_LEN     460
#define A_START      0
#define B_START      460
extern "C" void app_main()
{
   SIMPLEX.begin();
   STR.begin();

   //STR.SetCommunicator(UARTCTR::CreateInstance());

   ESPNowCore::CreateInstance()->BroadcastPing();

   STR.AddSetting(Setting::Type::Trigger, nullptr, 0, "LEDMODULE");

   LedModule module(GPIO_NUM_13, ALL_LED_LEN);

   auto simonWin = module.AddForeColorZone(A_START, ALL_LED_LEN, RGB(0, 255, 0), "SimonWin");
   auto simonLoose = module.AddForeColorZone(A_START, ALL_LED_LEN, RGB(255, 0, 0), "SimonLoose");

   FadingTransition fadeSimonWin("5SimonWin");
   FadingTransition fadeSimonLoose("6SimonLoose");

   simonWin->fTransition = &fadeSimonWin;
   simonLoose->fTransition = &fadeSimonLoose;

   /*auto simonA = module.AddForeColorZone(A_START, ZONE_LEN, RGB(0, 255, 0), "SimonA");
   auto simonB = module.AddForeColorZone(B_START, ZONE_LEN, RGB(0, 255, 0), "SimonB");

   FadingTransition fadeSimonA("7SimonA");
   FadingTransition fadeSimonB("8SimonB");

   simonA->fTransition = &fadeSimonA;
   simonB->fTransition = &fadeSimonB;*/

   auto baseA = module.AddForeColorZone(A_START, ZONE_LEN, RGB(0, 0, 255), "BaseA");
   auto baseB = module.AddForeColorZone(B_START, ZONE_LEN, RGB(255, 0, 0), "BaseB");

   LoadingTransition baseTransi("0BaseTransi");

   baseTransi.SetRate(255);

   baseA->fTransition = &baseTransi;
   baseB->fTransition = &baseTransi;

   FadingTransition fadeBaseA("1FadeBaseATransi");
   FadingTransition fadeBaseB("2FadeBaseBTransi");

   STR.AddSetting(Setting::Type::Trigger, nullptr, 0, "SetBaseLoading", [baseA, baseB, &baseTransi]() {
      baseA->fTransition = &baseTransi;
      baseB->fTransition = &baseTransi;
   });

   STR.AddSetting(Setting::Type::Trigger, nullptr, 0, "SetBaseFadeing",[baseA, baseB, &fadeBaseA, &fadeBaseB]() {
      baseA->fTransition = &fadeBaseA;
      baseB->fTransition = &fadeBaseB;
   });

   auto resetColor = module.AddBackColorZone(A_START, ALL_LED_LEN, RGB(0, 0, 0), "reset1");

   LoadingTransition reset1("3Reset1Transi");

   resetColor->fTransition = &reset1;

   auto blinkA = module.AddBiColorZone(A_START, ZONE_LEN, RGB(0, 0, 0), RGB(0, 255, 0), "BlinkA");
   auto blinkB = module.AddBiColorZone(B_START, ZONE_LEN, RGB(0, 255, 0), RGB(0, 0, 0), "BlinkB");

   BlinkingTransition blink("4BlinkTransi");

   blinkA->fTransition = &blink;
   blinkB->fTransition = &blink;

   auto blackBase = module.AddBiColorZone(A_START, ALL_LED_LEN, RGB(0, 0, 0), RGB(0, 0, 0), "blackBaseZone");

   while(true)
   {
      STR.Update();

      module.Update();
   }
}