
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

void setOutput( uint8_t Address, uint8_t functions )
{
    if( address > 3) return ; // discard other addresses

    uint8_t number = 1;
    if( functions & F5_F8 ) number = 5 ;
    // address == 1 && F1-F4 -> number range =  1 -  4 -> points  1  -  4
    // address == 2 && F1-F4 -> number range = 11 - 14 -> points  9  -  12
    // address == 3 && F1-F4 -> number range = 21 - 24 -> relay   1  -  4
    // address == 1 && F5-F8 -> number range =  1 -  4 -> points  1  -  4
    // address == 2 && F5-F8 -> number range = 11 - 14 -> points  9  -  12
    // address == 3 && F5-F8 -> number range = 21 - 24 -> relay   1  -  4
    

    for( int bitMask = 0b001 ; bitMask < 0b1000 ; bitMask <<= 1 )
    {
        if( (functions & bitMask) != (prevStates[ Address ] & bitMask) )        // check all 4 bits for F1 - F4, if atleast 1 bit has changed
        {
            prevStates[ Address ] = functions & 0x0F ;

            uint8_t ioNumber = number + ((Address - 1) * 10) ;
            uint8_t state ;
            if( functions & bitMask )   state = 1 ;    // on
            else                        state = 0 ;    // off

            setOutput( ioNumber, state ) ;

            return ;
        }

        number ++ ;
    }
}

void notifyXNetLocoFunc1( uint16_t Address, uint8_t Func1 )  //Gruppe1 0 0 0 F0 F4 F3 F2 F1
{
    setOutput( Address, Func1, F1-F4 ) ;
}

   
void notifyXNetLocoFunc2( uint16_t Address, uint8_t Func2 )  //Gruppe2 0000 F8 F7 F6 F5
{
    setOutput( Address, Func2 | F5-F8 ) ;
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