#include "System.hpp"
#include <exception>
#include <FastLED.h>
#define NUM_LEDS 12
#define DATA_PIN 13
#define BRIGHTNESS  200
#define FRAMES_PER_SECOND 60
bool gReverseDirection = false;
CRGBPalette16 gPal;
static CRGB leds[NUM_LEDS];
static uint8_t heat[NUM_LEDS];
void Service::LEDs::Initialize()
{
    // Uncomment/edit one of the following lines for your leds arrangement.
    // ## Clockless types ##
    FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);  // GRB ordering is assumed
}
void Service::LEDs::Handle(const uint8_t arg[]){
    /**
     * Handle arg packet.
     */

    switch(arg[0])
    {
        case ADD_TO_BLINK_COLOR_OPCODE:
        {
            for(uint8_t i=0;i<NUM_LEDS;i++)
                leds[i] = leds[i].addToRGB(arg[1]);
            FastLED.show();
            break;
        }
        default:
        {
            Logger::Log("[Service::%s]::%s():\t%x.\tNYI.", mName.c_str(), __func__, arg[0]);
            break;
        }
    };
}



/**
 * Build the static members on the RTOS::ActiveObject
 */
namespace Service
{
    using               _LEDs = RTOS::ActiveObject<Service::LEDs>;

    template <>
    const std::string   _LEDs::mName = std::string("LEDs");
    template <>
    uint8_t             _LEDs::mCountLoops = 0;
    template <>
    const uint8_t       _LEDs::mInputQueueItemLength = 16;
    template <>
    const uint8_t       _LEDs::mInputQueueItemSize = sizeof(uint16_t);
    template <>
    const size_t        _LEDs::mInputQueueSizeBytes = 
                                        RTOS::ActiveObject<Service::LEDs>::mInputQueueItemLength 
                                        * RTOS::ActiveObject<Service::LEDs>::mInputQueueItemSize;
    template <>
    uint8_t             _LEDs::mInputQueueAllocation[
                                        RTOS::ActiveObject<Service::LEDs>::mInputQueueSizeBytes
                                    ] = { 0 };
    template <>
    QueueHandle_t       _LEDs::mInputQueue = xQueueCreate(
                                        RTOS::ActiveObject<Service::LEDs>::mInputQueueItemLength,
                                        RTOS::ActiveObject<Service::LEDs>::mInputQueueItemSize
                                    );
    template <>
    TaskHandle_t        _LEDs::mHandle = 0;
    template <>
    uint8_t             _LEDs::mReceivedMsg[
                                        RTOS::ActiveObject<Service::LEDs>::mInputQueueItemSize
                                    ] = { 0 };
}

/** 
 * Build specific Service static members
 *  Otherwise it complains that the RTOS 
 * activeObject does not have these members, which is true.
*/
namespace Service
{
}



// Fire2012 by Mark Kriegsman, July 2012
// as part of "Five Elements" shown here: http://youtu.be/knWiGsmgycY
//// 
// This basic one-dimensional 'fire' simulation works roughly as follows:
// There's a underlying array of 'heat' cells, that model the temperature
// at each point along the line.  Every cycle through the simulation, 
// four steps are performed:
//  1) All cells cool down a little bit, losing heat to the air
//  2) The heat from each cell drifts 'up' and diffuses a little
//  3) Sometimes randomly new 'sparks' of heat are added at the bottom
//  4) The heat from each cell is rendered as a color into the leds array
//     The heat-to-color mapping uses a black-body radiation approximation.
//
// Temperature is in arbitrary units from 0 (cold black) to 255 (white hot).
//
// This simulation scales it self a bit depending on NUM_LEDS; it should look
// "OK" on anywhere from 20 to 100 LEDs without too much tweaking. 
//
// I recommend running this simulation at anywhere from 30-100 frames per second,
// meaning an interframe delay of about 10-35 milliseconds.
//
// Looks best on a high-density LED setup (60+ pixels/meter).
//
//
// There are two main parameters you can play with to control the look and
// feel of your fire: COOLING (used in step 1 above), and SPARKING (used
// in step 3 above).
//
// COOLING: How much does the air cool as it rises?
// Less cooling = taller flames.  More cooling = shorter flames.
// Default 55, suggested range 20-100 
#define COOLING  55

// SPARKING: What chance (out of 255) is there that a new spark will be lit?
// Higher chance = more roaring fire.  Lower chance = more flickery fire.
// Default 120, suggested range 50-200.
#define SPARKING 120


void Service::LEDs::Fire2012WithPalette()
{

    // Step 1.  Cool down every cell a little
    for( int i = 0; i < NUM_LEDS; i++) {
        heat[i] = qsub8( heat[i],  random8(0, ((COOLING * 10) / NUM_LEDS) + 2));
    }
  
    // Step 2.  Heat from each cell drifts 'up' and diffuses a little
    for( int k= NUM_LEDS - 1; k >= 2; k--) {
        heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2] ) / 3;
    }
    
    // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
    if( random8() < SPARKING ) {
        int y = random8(7);
        heat[y] = qadd8( heat[y], random8(160,255) );
    }

    // Step 4.  Map from heat cells to LED colors
    for( int j = 0; j < NUM_LEDS; j++) {
        // Scale the heat value from 0-255 down to 0-240
        // for best results with color palettes.
        uint8_t colorindex = scale8( heat[j], 240);
        CRGB color = ColorFromPalette( gPal, colorindex);
        int pixelnumber;
        if( gReverseDirection ) {
            pixelnumber = (NUM_LEDS-1) - j;
        } else {
            pixelnumber = j;
        }
        leds[pixelnumber] = color;
    }
}
