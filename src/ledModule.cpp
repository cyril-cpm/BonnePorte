#include "ledModule.h"
#include "esp_log.h"
#include "CustomType.hpp"
#include "freertos/freertos.h"
#include "freertos/task.h"
#include "Simplex.h"

static char* newStrCpy(const char* str)
{
    if (!str)
        return nullptr;
    
    int i = 0;
    while (str[i])
        i++;

    char *dst = nullptr;

    dst = new char[i+1];

    dst[i] = '\0';

    for (auto j = 0; j < i; j++)
        dst[j] = str[j];

    return dst;
}

static char* newStrCat(const char* a, const char* b)
{
    if (!a || !b)
        return nullptr;

    int i = 0;
    for (; a[i]; i++);

    int j = 0;
    for (; b[j]; j++);

    char* dst = nullptr;

    dst = new char[i+j+1];

    dst[i+j] = '\0';

    for (auto k = 0; k < i; k++)
        dst[k] = a[k];

    for (auto k = 0; k < j; k++)
        dst[k + i] = b[k];

    return dst;
}

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
        char* name = newStrCat(ledZone->fName, "_NBLED");
        STR_UInt16Ref(ledZone->fNbLed, name);
        delete[] name;

        name = newStrCat(ledZone->fName, "_STARTINDEX");
        STR_UInt16Ref(ledZone->fStartIndex, name);
        delete[] name;
    }
}

LedZone* LedModule::AddForeColorZone(uint16_t startIndex, uint16_t numLed, RGB color, const char* name, transition_type_t transition)
{
    LedZone* newZone = new LedZone{
        .fNbLed = numLed,
        .fStartIndex = startIndex,
        .fForeColor = color,
        .fBicolor = false,
        .fMonoFore = true,
        .fMonoBack = false,
        .fName = newStrCpy(name)
    };

    _initSTR(newZone);

    fLedZones.push_back(newZone);

    return fLedZones.back();
}

LedZone* LedModule::AddBackColorZone(uint16_t startIndex, uint16_t numLed, RGB color, const char* name, transition_type_t transition)
{
    LedZone* newZone = new LedZone{
        .fNbLed = numLed,
        .fStartIndex = startIndex,
        .fBackColor = color,
        .fBicolor = false,
        .fMonoFore = false,
        .fMonoBack = true,
        .fName = newStrCpy(name)
    };

    fLedZones.push_back(newZone);
    
    return fLedZones.back();
}

LedZone* LedModule::AddBiColorZone(uint16_t startIndex, uint16_t numLed, RGB foreColor, RGB backColor, const char* name, transition_type_t transition)
{
    LedZone* newZone = new LedZone{
        .fNbLed = numLed,
        .fStartIndex = startIndex,
        .fForeColor = foreColor,
        .fBackColor = backColor,
        .fBicolor = true,
        .fName = newStrCpy(name)
    };

    _initSTR(newZone);

    fLedZones.push_back(newZone);

    return fLedZones.back();
}

void    LedModule::SetLedColor(size_t i, RGB color)
{
    if (i < fNumLed)
        fLedData[i] = color;
}

RGB     LedModule::GetLedColor(size_t i)
{
    if (i > fNumLed)
        i = 0;
    return fLedData[i];
}

void    LedModule::Update()
{
    for (auto i = fLedZones.rbegin(); i != fLedZones.rend(); i++)
    {
        /*(*i)->fTransitionVRate.Update();

        uint32_t rateFactor = (1u << 16) / (*i)->fNbLed;

        switch ((*i)->fTransitionType)
        {
            
            case TRANSITION_TYPE_FADING:
                if ((*i)->fBicolor)
                {
                    RGB color;
                    RGB foreColor = (*i)->fForeColor;
                    RGB backColor = (*i)->fBackColor;
                    uint8_t rate = (*i)->fTransitionVRate;
                    uint8_t invRate = 255 - rate;

                    color.r = ((foreColor.r * rate) >> 8) + ((backColor.r * invRate) >> 8);
                    color.g = ((foreColor.g * rate) >> 8) + ((backColor.g * invRate) >> 8);
                    color.b = ((foreColor.b * rate) >> 8) + ((backColor.b * invRate) >> 8);

                    for (auto ledIndex = 0; ledIndex < (*i)->fNbLed; ledIndex++)
                        fLedData[ledIndex + (*i)->fStartIndex] = color;
                }
                else if ((*i)->fMonoFore)
                {
                    RGB color;
                    uint8_t invRate = 255 - (*i)->fTransitionVRate;

                    color.r = (((*i)->fForeColor.r * (*i)->fTransitionVRate) >> 8);
                    color.g = (((*i)->fForeColor.g * (*i)->fTransitionVRate) >> 8);
                    color.b = (((*i)->fForeColor.b * (*i)->fTransitionVRate) >> 8);

                    for (auto ledIndex = 0; ledIndex < (*i)->fNbLed; ledIndex++)
                    {
                        auto offsetedLedIndex = ledIndex + (*i)->fStartIndex;

                        color.r += (fLedData[offsetedLedIndex].r * invRate) >> 8;
                        color.g += (fLedData[offsetedLedIndex].g * invRate) >> 8;
                        color.b += (fLedData[offsetedLedIndex].b * invRate) >> 8;

                        fLedData[ledIndex + (*i)->fStartIndex] = color;
                    }
                }
                else if ((*i)->fMonoBack)
                {
                    RGB color;
                    uint8_t invRate = 255 - (*i)->fTransitionVRate;

                    color.r = (((*i)->fBackColor.r * invRate) >> 8);
                    color.g = (((*i)->fBackColor.g * invRate) >> 8);
                    color.b = (((*i)->fBackColor.b * invRate) >> 8);

                    for (auto ledIndex = 0; ledIndex < (*i)->fNbLed; ledIndex++)
                    {
                        auto offsetedLedIndex = ledIndex + (*i)->fStartIndex;

                        color.r += (fLedData[offsetedLedIndex].r * (*i)->fTransitionVRate) >> 8;
                        color.g += (fLedData[offsetedLedIndex].g * (*i)->fTransitionVRate) >> 8;
                        color.b += (fLedData[offsetedLedIndex].b * (*i)->fTransitionVRate) >> 8;

                        fLedData[ledIndex + (*i)->fStartIndex] = color;
                    }
                }
                break;
            
            case TRANSITION_TYPE_BLINKING:
                {
                    uint8_t blinkingSpeed = 255;
                    uint8_t blinkingRatio = 127;
                }
                break;
            
            case TRANSITION_TYPE_FLICKERING:
                {

                }
                break;
            case TRANSITION_TYPE_SIMPLEX_SLICE:
                {
                    _iq16 y = _IQ16(xTaskGetTickCount()/10);

                    //ESP_LOGI("NOISE", "START");
                    for (auto ledIndex = 0; ledIndex < (*i)->fNbLed; ledIndex++)
                    {
                        uint8_t noiseLevel = SIMPLEX.Noise(_IQ16mpy(_IQ16(ledIndex), _IQ16(0.09)), _IQ16mpy(y, _IQ16(0.09)));

                        if (noiseLevel > (*i)->fTransitionVRate)
                            fLedData[ledIndex + (*i)->fStartIndex] = (*i)->fForeColor;
                        else
                            fLedData[ledIndex + (*i)->fStartIndex] = (*i)->fBackColor;
                    }
                }
                break;
            case TRANSITION_TYPE_SIMPLEX_FADE:
                {
                    _iq16 y = _IQ16(xTaskGetTickCount()/10);

                    //ESP_LOGI("NOISE", "START");
                    for (auto ledIndex = 0; ledIndex < (*i)->fNbLed; ledIndex++)
                    {
                        uint8_t noiseLevel = SIMPLEX.Noise(_IQ16mpy(_IQ16(ledIndex), _IQ16(0.09)), _IQ16mpy(y, _IQ16(0.09)));

                        RGB color;
                        color.r = (((*i)->fForeColor.r * noiseLevel) >> 8) + (((*i)->fBackColor.r * (255 - noiseLevel)) >> 8);
                        color.g = (((*i)->fForeColor.g * noiseLevel) >> 8) + (((*i)->fBackColor.g * (255 - noiseLevel)) >> 8);
                        color.b = (((*i)->fForeColor.b * noiseLevel) >> 8) + (((*i)->fBackColor.b * (255 - noiseLevel)) >> 8);
                        fLedData[ledIndex + (*i)->fStartIndex] = color;
                    }
                }
                break;
            default:
                break;
        }*/
    
        if ((*i)->fTransition)
        {
            (*i)->fTransition->Apply(this, *i);
        }
    }

    FLed.show();
}

Transition::Transition(const char* name)
{
    fName = newStrCpy(name);
}

LoadingTransition::LoadingTransition(const char* name) : Transition(name)
{
    fRate.fName = newStrCat(name, "_RATE");
    fRate.fRate = 0;

    fRate.InitSTR();

    char* settingName = newStrCat(name, "_DIRECTION");
    STR_UInt8Ref(fDirection, settingName);
    delete[] settingName;
}

void LoadingTransition::Apply(LedModule* module, LedZone* zone)
{
    fRate.Update();
    uint32_t rateFactor = (1u << 16) / zone->fNbLed;

    for (auto ledIndex = 0; ledIndex < zone->fNbLed; ledIndex++)
    {
       
        uint8_t ratedLed = (ledIndex * rateFactor) >> 8;
        int resolvedIndex = zone->fStartIndex;
        int invResolvedIndex = zone->fStartIndex;

        switch (fDirection)
        {
        case BEGIN_END:
            resolvedIndex += ledIndex;
            break;
        
        case END_BEGIN:
            resolvedIndex += zone->fNbLed - 1 - ledIndex;
            break;

        case MID_EXT:
            resolvedIndex += (zone->fNbLed - 1 - ledIndex) >> 1;
            invResolvedIndex += (ledIndex >> 1) + (zone->fNbLed >> 1);
            break;

        case EXT_MID:
            resolvedIndex += ledIndex >> 1;
            invResolvedIndex += ((zone->fNbLed - 1 - ledIndex) >> 1) + (zone->fNbLed >> 1);
        default:
            break;
        }
        
        if (!fRate.fRate || ratedLed > fRate.fRate)
        {
            if (zone->fBicolor)
            {
                module->SetLedColor(resolvedIndex, zone->fBackColor);

                if (fDirection == MID_EXT || fDirection == EXT_MID)
                    module->SetLedColor(invResolvedIndex, zone->fBackColor);
            }
            else if (zone->fMonoBack)
            {
                module->SetLedColor(resolvedIndex, zone->fBackColor);

                if (fDirection == MID_EXT || fDirection == EXT_MID)
                    module->SetLedColor(invResolvedIndex, zone->fBackColor);
            }
        }
        else
        {
            if (zone->fBicolor)
            {
                module->SetLedColor(resolvedIndex, zone->fForeColor);

                if (fDirection == MID_EXT || fDirection == EXT_MID)
                    module->SetLedColor(invResolvedIndex, zone->fForeColor);
            }

            else if (zone->fMonoFore)
            {
                module->SetLedColor(resolvedIndex, zone->fForeColor);

                if (fDirection == MID_EXT || fDirection == EXT_MID)
                    module->SetLedColor(invResolvedIndex, zone->fForeColor);
            }
        }
    }
}

FadingTransition::FadingTransition(const char* name) : Transition(name)
{
    fRate.fName = newStrCat(name, "_RATE");
    fRate.fRate = 0;

    fRate.InitSTR();
}

void FadingTransition::Apply(LedModule* module, LedZone* zone)
{
    fRate.Update();
    
    if (zone->fBicolor)
    {
        RGB color;
        RGB foreColor = zone->fForeColor;
        RGB backColor = zone->fBackColor;
        uint8_t rate = fRate.fRate;
        uint8_t invRate = 255 - rate;

        color.r = ((foreColor.r * rate) >> 8) + ((backColor.r * invRate) >> 8);
        color.g = ((foreColor.g * rate) >> 8) + ((backColor.g * invRate) >> 8);
        color.b = ((foreColor.b * rate) >> 8) + ((backColor.b * invRate) >> 8);

        for (auto ledIndex = 0; ledIndex < zone->fNbLed; ledIndex++)
            module->SetLedColor(ledIndex + zone->fStartIndex, color);
    }
    else if (zone->fMonoFore)
    {
        RGB color;
        uint8_t invRate = 255 - fRate.fRate;

        color.r = ((zone->fForeColor.r * fRate.fRate) >> 8);
        color.g = ((zone->fForeColor.g * fRate.fRate) >> 8);
        color.b = ((zone->fForeColor.b * fRate.fRate) >> 8);

        for (auto ledIndex = 0; ledIndex < zone->fNbLed; ledIndex++)
        {
            auto offsetedLedIndex = ledIndex + zone->fStartIndex;

            color.r += (module->GetLedColor(offsetedLedIndex).r * invRate) >> 8;
            color.g += (module->GetLedColor(offsetedLedIndex).g * invRate) >> 8;
            color.b += (module->GetLedColor(offsetedLedIndex).b * invRate) >> 8;

            module->SetLedColor(ledIndex + zone->fStartIndex, color);
        }
    }
    else if (zone->fMonoBack)
    {
        RGB color;
        uint8_t invRate = 255 - fRate.fRate;

        color.r = ((zone->fBackColor.r * invRate) >> 8);
        color.g = ((zone->fBackColor.g * invRate) >> 8);
        color.b = ((zone->fBackColor.b * invRate) >> 8);

        for (auto ledIndex = 0; ledIndex < zone->fNbLed; ledIndex++)
        {
            auto offsetedLedIndex = ledIndex + zone->fStartIndex;

            color.r += (module->GetLedColor(offsetedLedIndex).r * fRate.fRate) >> 8;
            color.g += (module->GetLedColor(offsetedLedIndex).g * fRate.fRate) >> 8;
            color.b += (module->GetLedColor(offsetedLedIndex).b * fRate.fRate) >> 8;

            module->SetLedColor(ledIndex + zone->fStartIndex, color);
        }
    }
}