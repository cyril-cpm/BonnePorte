#include "esp_log.h"
#include "transitionValue.h"
#include "freertos/freertos.h"
#include "freertos/task.h"
#include "Settingator.h"
#include "CustomType.hpp"

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

void TransitionValue::Update()
{
    if (fTransitioning)
    {
        uint32_t deltaTick = (xTaskGetTickCount() - fTickStamp);

        uint16_t newRate = 0;

        newRate = fBegin + ( (fForward) ? ((fRatio * deltaTick) >> 16) : -((fRatio * deltaTick) >> 16));

        if (newRate > 255)
            newRate = fForward ? 255 : 0;
        
        if ((fForward && newRate >= fEnd) || (!fForward && newRate <= fEnd))
        {
            fTransitioning = false;
            fRate = fEnd;
        } 
        else
            fRate = newRate;

        //ESP_LOGI("TR", "Rate : %d", fRate);
    }
}

void TransitionValue::InitSTR()
{
    char* name = newStrCat(fName, "_BEGIN");
    STR_UInt8Ref(fBegin, name);
    delete[] name;

    name = newStrCat(fName, "_END");
    STR_UInt8Ref(fEnd, name);
    delete[] name;

    name = newStrCat(fName, "_TIME");
    STR_UInt32Ref(fTime, name);
    delete[] name;

    name = newStrCat(fName, "_TRANSITION");
    STR.AddSetting(Setting::Type::Trigger, nullptr, 0, name, [ptr = this]() {
        if (ptr)
        {   
            ptr->fTransitioning = true;
            ptr->fForward = ptr->fBegin < ptr->fEnd;
            ptr->fTickStamp = xTaskGetTickCount();
            ptr->fDeltaRate = abs(ptr->fBegin - ptr->fEnd);
            ptr->fRatio = ((uint32_t)(ptr->fDeltaRate) << 16) / ptr->fTime;
        }
    });
    delete[] name;
}

TransitionValue::operator uint8_t&()
{
    return fRate;
}
