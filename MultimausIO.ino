
#include "eeprom.h"
#include "XpressNetMaster.h"
#include "macros.h"
#include "turnouts.h"

const int NA = 0 ;

XpressNetMasterClass  Xnet ;
Adafruit_PWMServoDriver servoDriver ;

uint8_t prevStates[6] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff } ;

void notifyXNetLocoDrive28( uint16_t Address, uint8_t Speed )  // N.B. it may be that I need other LocoDriveXX functions as well, depends on what multimaus is going to do.
{

}

void notifyXNetLocoFunc1( uint16_t Address, uint8_t Func1 )  //Gruppe1 0 0 0 F0 F4 F3 F2 F1
{
    // address == 1 -> number range =  1 -  4 -> points  1  -  4
    // address == 2 -> number range = 11 - 14 -> points  9  -  12
    // address == 3 -> number range = 21 - 24 -> relay   1  -  4
    if( address > 3) return ;

    for( int bitMask = 0b001 ; bitMask < 0b1000 ; bitMask <<= 1 )
    {
        if( (Func1 & bitMask) != (prevStates[ Address ] & bitMask) )        // check all 4 bits for F1 - F4, if atleast 1 bit has changed
        {
            prevStates[ Address ] = Func1 & 0x0F ;
        }
    }
}
void notifyXNetLocoFunc2( uint16_t Address, uint8_t Func2 )  //Gruppe2 0000 F8 F7 F6 F5
{
    // address == 1 -> number range =  5 -  8 -> points  5  -  8
    // address == 2 -> number range = 15 - 18 -> points 13  - 16
    // address == 3 -> number range = 25 - 28 -> relay   5  -  8
    if( address > 3) return ;

    for( int bitMask = 0b001 ; bitMask < 0b1000 ; bitMask <<= 1 )
    {
        if( (Func2 & bitMask) != (prevStates[Address + 3] & bitMask) )        // check all 4 bits for F5 - F4
        {
            prevStates[ Address + 3 ] = Func2 & 0x0F ;                          // store the change
        }
    }
    
}

void setup()
{
    initTurnouts() ;
    Xnet.setup( Loco28, dirPin ) ; // N.B. may need to change depending on what multimaus will do.

    pinMode( relayPin1, OUTPUT ) ;
    pinMode( relayPin2, OUTPUT ) ;
    pinMode( relayPin3, OUTPUT ) ;
    pinMode( relayPin4, OUTPUT ) ;
    pinMode( relayPin5, OUTPUT ) ;
    pinMode( relayPin6, OUTPUT ) ;
    pinMode( relayPin7, OUTPUT ) ;
    pinMode( relayPin8, OUTPUT ) ;
}

void loop()
{
    Xnet.update() ;
}