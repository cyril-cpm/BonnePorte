#include "Settingator.h"
#include "UARTCommunicator.h"
#include "ESPNowCommunicator.h"
#include "Led.h"
#include "ledModule.h"
#include "CustomType.hpp"
#include "esp_log.h"
#include "Simplex.h"

Settingator STR(nullptr);

/*#define ALL_LED_LEN  920
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

   auto simonA = module.AddForeColorZone(A_START, ZONE_LEN, RGB(0, 255, 0), "SimonA");
   auto simonB = module.AddForeColorZone(B_START, ZONE_LEN, RGB(0, 255, 0), "SimonB");

   FadingTransition fadeSimonA("7SimonA");
   FadingTransition fadeSimonB("8SimonB");

   simonA->fTransition = &fadeSimonA;
   simonB->fTransition = &fadeSimonB;*/

   /*auto baseA = module.AddForeColorZone(A_START, ZONE_LEN, RGB(0, 0, 255), "BaseA");
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
}*/

#define ALL_LED_LEN  238
#define ROD_LEN     55
#define BL_START    0
#define FL_START    64
#define FR_START    119
#define BR_START    183
#define SL_START    55
#define SR_START    174
#define SPOT_LEN 9

extern "C" void app_main()
{
   SIMPLEX.begin();
   STR.begin();

   //STR.SetCommunicator(UARTCTR::CreateInstance());

   ESPNowCore::CreateInstance()->BroadcastPing();

   STR.AddSetting(Setting::Type::Trigger, nullptr, 0, "LEDMODULE");

   LedModule module(GPIO_NUM_0, ALL_LED_LEN);

   ZoneStack introStack("IntroStack");

   auto wholeZoneBG = LedZone::CreateBackColorZone(0, ALL_LED_LEN, RGB(0, 0, 0), "AllBG");

   auto blIntro = LedZone::CreateForeColorZone(BL_START, ROD_LEN, RGB(0, 0, 255), "introBL");
   auto flIntro = LedZone::CreateForeColorZone(FL_START, ROD_LEN, RGB(0, 0, 255), "introFL");
   auto frIntro = LedZone::CreateForeColorZone(FR_START, ROD_LEN, RGB(0, 0, 255), "introFR");
   auto brIntro = LedZone::CreateForeColorZone(BR_START, ROD_LEN, RGB(0, 0, 255), "introBR");

   LoadingTransition introForwardTransi("8INTRO_FORWARD_TRANSI");
   introForwardTransi.SetDirection(LoadingTransition::BEGIN_END);

   LoadingTransition introBackwardTransi("9INTRO_BACKWARD_TRANSI");
   introBackwardTransi.SetDirection(LoadingTransition::END_BEGIN);

   blIntro->fTransition = &introForwardTransi;
   frIntro->fTransition = &introForwardTransi;

   flIntro->fTransition = &introBackwardTransi;
   brIntro->fTransition = &introBackwardTransi;

   introStack.Append(blIntro);
   introStack.Append(flIntro);
   introStack.Append(frIntro);
   introStack.Append(brIntro);
   introStack.Append(wholeZoneBG);

   module.SetZoneStack(&introStack);

   STR.AddSetting(Setting::Type::Trigger, nullptr, 0, "INTRO_STACK", [&introStack, &module](){
        module.SetZoneStack(&introStack);
   });

   while(true)
   {
      STR.Update();

      module.Update();
   }
}