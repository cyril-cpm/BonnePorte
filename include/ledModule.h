#pragma once

#include "Led.h"
#include "CustomType.hpp"
#include "transitionValue.h"
#include <vector>

typedef enum {
    TRANSITION_TYPE_LOADING         = 1,
    TRANSITION_TYPE_FADING          = 2,
    TRANSITION_TYPE_BLINKING        = 3,
    TRANSITION_TYPE_FLICKERING      = 4,
    TRANSITION_TYPE_SIMPLEX_SLICE   = 5,
    TRANSITION_TYPE_SIMPLEX_FADE    = 6
} transition_type_t;

class LedModule;
class LedZone;

class Transition
{
    public:
    Transition(const char* name);
    virtual void Apply(LedModule* module, LedZone* zone) = 0;

    private:
    Transition();
    char*   fName;
};

class LoadingTransition : public Transition
{
    public:
    LoadingTransition(const char* name);
    virtual void Apply(LedModule* module, LedZone* zone);

    typedef enum {
        BEGIN_END = 0,
        END_BEGIN = 1,
        MID_EXT = 2,
        EXT_MID = 3
    } LoadDirection;

    private:
    LoadingTransition();
    TransitionValue fRate;
    uint8_t   fDirection = BEGIN_END;
};

class FadingTransition : public Transition
{
    public:
    FadingTransition(const char* name);
    virtual void Apply(LedModule* module, LedZone* zone);

    private:
    FadingTransition();
    TransitionValue fRate;
};

struct LedZone
{
    public:
    uint16_t            fNbLed = 0;
    uint16_t            fStartIndex;
    RGB                 fForeColor = RGB(0, 0, 0);
    RGB                 fBackColor = RGB(0, 0, 0);
    bool                fBicolor = false;
    bool                fMonoFore = false;
    bool                fMonoBack = false;
    
    char*               fName;

    Transition*         fTransition = nullptr;
    
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

    LedZone* AddForeColorZone(uint16_t startIndex, uint16_t numLed, RGB color, const char* name, transition_type_t transition = TRANSITION_TYPE_LOADING);
    LedZone* AddBackColorZone(uint16_t startIndex, uint16_t numLed, RGB color, const char* name, transition_type_t transition = TRANSITION_TYPE_LOADING);
    LedZone* AddBiColorZone(uint16_t startIndex, uint16_t numLed, RGB foreColor, RGB backColor, const char* name, transition_type_t transition = TRANSITION_TYPE_LOADING);

    void    SetLedColor(size_t i, RGB color);
    RGB     GetLedColor(size_t i);

    void Update();
};