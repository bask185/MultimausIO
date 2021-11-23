#include "io.h"
#include "eeprom.h"
#include "XpressNetMaster.h"
#include "macros.h"
#include "turnouts.h"
#include "weistra.h"

const int F1_F4 = 0 ;
const int F5_F8 = 0x80 ;

const int ACCEL_FACTOR  =    250 ;
const int DECEL_FACTOR  =    100 ;
const int SPEED_MAX     =    100 ;
const int SPEED_MIN     =     10 ;

uint16_t interval = 100 ;

XpressNetMasterClass    Xnet ;

Weistra throttle( enablePin ) ;

uint8_t prevStates[6] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff } ;
uint8_t setPoint = 0 ;
uint8_t currentSpeed = 0 ;

void notifyXNetLocoDrive28( uint16_t Address, uint8_t Speed )  // N.B. it may be that I need other LocoDriveXX functions as well, depends on what multimaus is going to do.
{
    if( Speed & 0x80 ) { digitalWrite( dirPin1, HIGH ) ;
                         digitalWrite( dirPin2,  LOW ) ; }
    else               { digitalWrite( dirPin1,  LOW ) ;
                         digitalWrite( dirPin2, HIGH ) ; }

    setPoint = Speed & 0x7F ;
    setPoint = map( setPoint, 0, 28, 0, 100 ) ;                 // map 28 speedsteps to 100 for weistra control
    setPoint = constrain( setPoint, SPEED_MIN, SPEED_MAX) ;     // constrain value to min and maximum settings
}

void updateSpeed()                                              // handles speed and acceleration/braking times
{
    REPEAT_MS( interval );
    static uint8_t prevSpeed ;
    if( setPoint > currentSpeed ) interval = ACCEL_FACTOR ;     // accelarating goes quicker..
    if( setPoint < currentSpeed ) interval = DECEL_FACTOR ;     // .. than braking

    if( currentSpeed < setPoint ) currentSpeed ++ ;
    if( currentSpeed > setPoint ) currentSpeed -- ;

    END_REPEAT
}

void setOutput( uint8_t Address, uint8_t functions )
{
    if( Address > 3) return ; // discard other addresses

    uint8_t number = 1 ;
    if( functions & F5_F8 ) number = 5 ;
    // for  F1-F4
    // address == 1 -> number range =  1 -  4 -> points  1  -  4
    // address == 2 -> number range = 11 - 14 -> points  9  - 12
    // address == 3 -> number range = 21 - 24 -> relay   1  -  4

    // for F5-F8 
    // address == 1 -> number range =  1 -  4 -> points  5  -  8
    // address == 2 -> number range = 11 - 14 -> points 13  - 16
    // address == 3 -> number range = 21 - 24 -> relay   5  -  8

    for( int bitMask = 0b001 ; bitMask < 0b1000 ; bitMask <<= 1 )
    {
        if( (functions & bitMask) != (prevStates[ Address ] & bitMask) )        // check all 4 bits for F1 - F4, if atleast 1 bit has changed
        {
            prevStates[ Address ] = functions & 0x0F ;

            uint8_t ioNumber = number + ((Address - 1) * 10) ;
            uint8_t state ;

            if( functions & bitMask ) state = 1 ;    // on
            else                      state = 0 ;    // off

            if( ioNumber <= 16 ) setTurnout( ioNumber, state ) ;
            else                 digitalWrite( relay[ioNumber-17], state ) ;

            return ;
        }

        number ++ ;
    }
}

void notifyXNetLocoFunc1( uint16_t Address, uint8_t Func1 ) { setOutput( Address, Func1 | F1_F4 ) ; } // called from Xnet library
void notifyXNetLocoFunc2( uint16_t Address, uint8_t Func2 ) { setOutput( Address, Func2 | F5_F8 ) ; }

void setup()
{
    initTurnouts() ;
    Xnet.setup( Loco28, max45dir ) ; // N.B. may need to change depending on what multimaus will do.

    for( int i = 0 ; i < 8 ; i ++ ) { pinMode( relay[i], OUTPUT ) ; }  // set all relay pins on output
}

void loop()
{
    Xnet.update() ;
    throttle.update() ;
    updateSpeed() ;
}