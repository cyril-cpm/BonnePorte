#include "ledModule.h"
#include "esp_log.h"
#include "CustomType.hpp"
#include "freertos/freertos.h"
#include "freertos/task.h"

LedModule::LedModule(gpio_num_t ledPin, uint16_t numLed)
{
    fNumLed = numLed;
    fLedPin = ledPin;
    fLedData = new RGB[numLed];

    FLed.addLeds(fLedPin, fLedData, numLed);
}

void    LedModule::_initSTR(LedZone* ledZone)
{
    if (ledZone)
    {
        STR_UInt16Ref(ledZone->fNbLed, "NB_LED");
        STR_UInt16Ref(ledZone->fStartIndex, "START_INDEX");
        STR_UInt8Ref(ledZone->fBegin, "BEGIN");
        STR_UInt8Ref(ledZone->fEnd, "END");
        STR_UInt32Ref(ledZone->fTime, "TIME");

        STR.AddSetting(Setting::Type::Trigger, nullptr, 0, "TRANSITION", [ledZone]() {
            if (ledZone)
            {
                ledZone->fTransitioning = true;
                ledZone->fForward = ledZone->fBegin < ledZone->fEnd;
                ledZone->fTickStamp = xTaskGetTickCount();
                ledZone->fDeltaRate = abs(ledZone->fBegin - ledZone->fEnd);
                ledZone->fRatio = ((uint32_t)(ledZone->fDeltaRate) << 16) / ledZone->fTime;
            }
        });
    }
}

LedZone* LedModule::AddForeColorZone(uint16_t startIndex, uint16_t numLed, RGB color, transition_type_t transition)
{
    LedZone* newZone = new LedZone{
        .fNbLed = numLed,
        .fStartIndex = startIndex,
        .fForeColor = color,
        .fTransitionType = transition,
        .fBicolor = false,
        .fMonoFore = true,
        .fMonoBack = false
    };

    _initSTR(newZone);

    fLedZones.push_back(newZone);

    return fLedZones.back();
}

LedZone* LedModule::AddBackColorZone(uint16_t startIndex, uint16_t numLed, RGB color, transition_type_t transition)
{
    LedZone* newZone = new LedZone{
        .fNbLed = numLed,
        .fStartIndex = startIndex,
        .fBackColor = color,
        .fTransitionType = transition,
        .fBicolor = false,
        .fMonoFore = false,
        .fMonoBack = true
    };

    fLedZones.push_back(newZone);
    
    return fLedZones.back();
}

LedZone* LedModule::AddBiColorZone(uint16_t startIndex, uint16_t numLed, RGB foreColor, RGB backColor, transition_type_t transition)
{
    LedZone* newZone = new LedZone{
        .fNbLed = numLed,
        .fStartIndex = startIndex,
        .fForeColor = foreColor,
        .fBackColor = backColor,
        .fTransitionType = transition,
        .fBicolor = true
    };

    _initSTR(newZone);

    fLedZones.push_back(newZone);

    return fLedZones.back();
}

void    LedModule::Update()
{
    for (auto i = fLedZones.rbegin(); i != fLedZones.rend(); i++)
    {
        if ((*i)->fTransitioning)
        {
            uint32_t deltaTick = (xTaskGetTickCount() - (*i)->fTickStamp);

            uint16_t newRate = 0;

            newRate = (*i)->fBegin + ( ((*i)->fForward) ? (((*i)->fRatio * deltaTick) >> 16) : -(((*i)->fRatio * deltaTick) >> 16));

            if (newRate > 255)
                newRate = (*i)->fForward ? 255 : 0;
            
            if (((*i)->fForward && newRate >= (*i)->fEnd) || (!(*i)->fForward && newRate <= (*i)->fEnd))
            {
                (*i)->fTransitioning = false;
                (*i)->fTransitionRate = (*i)->fEnd;
            } 
            else
                (*i)->fTransitionRate = newRate;
        }


        uint32_t rateFactor = (1u << 16) / (*i)->fNbLed;

        switch ((*i)->fTransitionType)
        {
            case TRANSITION_TYPE_LAODING:
                for (auto ledIndex = 0; ledIndex < (*i)->fNbLed; ledIndex++)
                {
                    //uint16_t ratedLed = (((uint16_t) ledIndex << 8) - ledIndex) /  (*i)->fNbLed; 
                    uint8_t ratedLed = (ledIndex * rateFactor) >> 8;
                    if (!(*i)->fTransitionRate || ratedLed > (*i)->fTransitionRate)
                    {
                        if ((*i)->fBicolor)
                            fLedData[ledIndex + (*i)->fStartIndex] = (*i)->fBackColor;
                        else if ((*i)->fMonoBack)
                            fLedData[ledIndex + (*i)->fStartIndex] = (*i)->fBackColor;
                    }
                    else
                    {
                        if ((*i)->fBicolor)
                            fLedData[ledIndex + (*i)->fStartIndex] = (*i)->fForeColor;
                        else if ((*i)->fMonoFore)
                            fLedData[ledIndex + (*i)->fStartIndex] = (*i)->fForeColor;
                    }
                }
                break;
            default:
                break;
        }
    }

    FLed.show();
}