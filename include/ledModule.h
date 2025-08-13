#pragma once

#include "Led.h"
#include "CustomType.hpp"
#include <vector>

typedef enum {
    TRANSITION_TYPE_LAODING         = 1,
    TRANSITION_TYPE_FADING          = 2,
    TRANSITION_TYPE_BLINKING        = 3,
    TRANSITION_TYPE_FLICKERING      = 4,
    TRANSITION_TYPE_SIMPLEX_SLICE   = 5,
    TRANSITION_TYPE_SIMPLEX_FADE    = 6
} transition_type_t;

struct LedZone
{
    public:
    uint16_t            fNbLed = 0;
    uint16_t            fStartIndex;
    RGB                 fForeColor = RGB(0, 0, 0);
    RGB                 fBackColor = RGB(0, 0, 0);
    uint8_t             fTransitionRate = 0;
    transition_type_t   fTransitionType = TRANSITION_TYPE_LAODING;
    bool                fBicolor = false;
    bool                fMonoFore = false;
    bool                fMonoBack = false;
    
    // Transition Stuff
    uint8_t             fBegin = 0;
    uint8_t             fEnd = 255;
    uint32_t            fTime = 100;
    
    bool                fTransitioning = false;
    bool                fForward = false;
    uint32_t            fTickStamp = 0;
    uint32_t            fRatio = 0;
    uint8_t             fDeltaRate = 0;
    
};

class LedModule
{

    private:
    LedModule();
    void    _initSTR(LedZone* ledZone);

    RGB*        fLedData;
    uint16_t      fNumLed = 0;
    gpio_num_t  fLedPin = GPIO_NUM_NC;

    std::vector<LedZone*>    fLedZones;

    public:
    LedModule(gpio_num_t ledPin, uint16_t numLed);

    LedZone* AddForeColorZone(uint16_t startIndex, uint16_t numLed, RGB color, transition_type_t transition = TRANSITION_TYPE_LAODING);
    LedZone* AddBackColorZone(uint16_t startIndex, uint16_t numLed, RGB color, transition_type_t transition = TRANSITION_TYPE_LAODING);
    LedZone* AddBiColorZone(uint16_t startIndex, uint16_t numLed, RGB foreColor, RGB backColor, transition_type_t transition = TRANSITION_TYPE_LAODING);

    void Update();
};