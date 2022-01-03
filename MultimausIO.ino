#include "io.h"
#include "eeprom.h"
#include "XpressNetMaster.h"
#include "macros.h"
#include "turnouts.h"
#include "weistra.h"
#include "routes.h"

const int F1_F4 = 0 ;
const int F5_F8 = 0x80 ;

const int ACCEL_FACTOR  =     30 ;
const int SPEED_MAX     =     70 ;
const int SPEED_MIN     =      1 ;
const int MAX_CURRENT   =     50 ;  // 1 Ampere over 0.5R -> 0.5V -> 0.5 / 5V * 1023 = 102
const int nSamples      =     10 ;  // *2ms 

uint16_t interval = 100 ;

XpressNetMasterClass    Xnet ;

Weistra throttle( enablePin, 50, 100 ) ; // 50-100Hz

uint8_t prevStates[6] = { 0, 0, 0, 0, 0, 0 } ;
int8_t setPoint = 0 ;
int8_t currentSpeed = 0 ;

uint8_t lookUpSpeed( uint8_t speed )
{
    switch( speed )
    {
    case 0b00000 : return  0 ;
    case 0b00010 : return  1 ;
    case 0b10010 : return  2 ;
    case 0b00011 : return  3 ;
    case 0b10011 : return  4 ;
    case 0b00100 : return  5 ;
    case 0b10100 : return  6 ;
    case 0b00101 : return  7 ;
    case 0b10101 : return  8 ;
    case 0b00110 : return  9 ;
    case 0b10110 : return 10 ;
    case 0b00111 : return 11 ;
    case 0b10111 : return 12 ;
    case 0b01000 : return 13 ;
    case 0b11000 : return 14 ;
    case 0b01001 : return 15 ;
    case 0b11001 : return 16 ;
    case 0b01010 : return 17 ;
    case 0b11010 : return 18 ;
    case 0b01011 : return 19 ;
    case 0b11011 : return 20 ;
    case 0b01100 : return 21 ;
    case 0b11100 : return 22 ;
    case 0b01101 : return 23 ;
    case 0b11101 : return 24 ;
    case 0b01110 : return 25 ;
    case 0b11110 : return 26 ;
    case 0b01111 : return 27 ;
    case 0b11111 : return 28 ;
    }
}
void notifyXNetLocoDrive28( uint16_t Address, uint8_t Speed )   // N.B. it may be that I need other LocoDriveXX functions as well, depends on what multimaus is going to do.
{
    setPoint = lookUpSpeed( Speed & 0b00011111 ) ;
    setPoint = map( setPoint, 0, 28, 0, SPEED_MAX ) ;           // map 28 speedsteps to 100 for weistra control
    if( Speed & 0x80 ) setPoint = -setPoint ;
}

void updateSpeed()                                              // handles speed and acceleration/braking times
{
    if( throttle.getState() == 0 ) return ;

    REPEAT_MS( ACCEL_FACTOR )
    {
        static int8_t speedPrev ;

        if( currentSpeed < setPoint ) currentSpeed ++ ;
        if( currentSpeed > setPoint ) currentSpeed -- ;

        if( currentSpeed < 0 ) { digitalWrite( dirPin1, HIGH ) ;
                                 digitalWrite( dirPin2,  LOW ) ; }
        if( currentSpeed > 0 ) { digitalWrite( dirPin1,  LOW ) ;
                                 digitalWrite( dirPin2, HIGH ) ; }

        if( currentSpeed != speedPrev )
        {
            speedPrev = currentSpeed ;
            throttle.setSpeed( abs(currentSpeed) ) ;
        }
    } END_REPEAT
}

void setOutput( uint8_t Address, uint8_t functions )
{
    if( Address == 3) return ; // address 3 is unused

    uint8_t number = 1 ;
    uint8_t indexShift = 0 ;
    if( functions & F5_F8 )
    {
        number = 5 ;
        indexShift = 3 ;
    }
    uint8_t ioNumber = number + ((Address - 1) * 10) ;

    // for  F1-F4
    // address == 1 -> number range =  1 -  4 -> points  1  -  4
    // address == 2 -> number range = 11 - 14 -> points  9  - 12
    // address == 3 -> number range = 21 - 24 -> relay   1  -  4

    // for F5-F8 
    // address == 1 -> number range =  1 -  4 -> points  5  -  8
    // address == 2 -> number range = 11 - 14 -> points 13  - 16
    // address == 3 -> number range = 21 - 24 -> relay   5  -  8

    for( int bitMask = 0b001 ; bitMask <= 0b1000 ; bitMask <<= 1 )
    {
        if( (functions & bitMask) != (prevStates[ Address+indexShift ] & bitMask) )        // check all 4 bits for F1 - F4, if atleast 1 bit has changed
        {
            prevStates[ Address+indexShift ] = functions & 0x0F ;

            uint8_t ioNumber = number + ((Address - 1) * 10) ;
            uint8_t state ;

            if( functions & bitMask ) state = 1 ;    // on
            else                      state = 0 ;    // off

            if(      ioNumber <=  8 ) setTurnout( ioNumber - 1 , state ) ;             //  1 <->  8
            else if( ioNumber <= 18 ) digitalWrite( relay[ioNumber-11], state^1 ) ;    // 11 <-> 18
                                                                                       // 21 <-> 28  address 3 not used.
            else if( ioNumber <= 38 ) setRoute( ioNumber - 31 ) ;                      // 31 <-> 38
            //else if( ioNumber <= 38 ) runProgram( ioNumber - 31)                     // 41 <-> 48

            return ;
        }

        number ++ ;
    }
}


void notifyXNetLocoFunc1( uint16_t Address, uint8_t Func1 ) { setOutput( Address, Func1 | F1_F4 ) ; } // called from Xnet library
void notifyXNetLocoFunc2( uint16_t Address, uint8_t Func2 ) { setOutput( Address, Func2 | F5_F8 ) ; }

void notifyXNetLocoFunc3( uint16_t Address, uint8_t Func )
{
    static uint8_t prevFunc = 0xFF ;

    if( (Func & 0b0100) != (prevFunc & 0b0100) ) { adjustServo( F11 ) ; }
    if( (Func & 0b1000) != (prevFunc & 0b1000) ) { adjustServo( F12 ) ; }

    prevFunc = Func ;
}

void notifyXNetPower(uint8_t State) 
{
    freeRoute() ;
    
    if( State == csNormal )
    {
        digitalWrite( ledPin, HIGH ) ; 
        throttle.setState( 1 ) ;
    }
    else
    {
        digitalWrite( ledPin,  LOW ) ; 
        throttle.setState( 0 ) ;                                                    // turn off track power
        currentSpeed = 0 ;                                                          // reset speed as well so train does not jump away when power is enabled.
        //setPoint = 0 ;                                                              // ensures that the maus's knob needs to be turned before train moves again
    }
}

void shortCircuit()
{
    if( digitalRead( enablePin ) == LOW ) return ;                                  // if PWM is on OFF cycle OR power is simply off we cannot measure trackpower -> return

    REPEAT_MS( 2 ) 
    {
        static uint8_t counter = 0 ;

        int sample = analogRead( CS ) ;

        if( sample <= MAX_CURRENT ) counter = nSamples ;

        if( --counter == 0 )
        {
            throttle.setState( 0 ) ;
            Xnet.setPower( csShortCircuit ) ;
        }
        
    } END_REPEAT ;
}

void setup()
{
    initTurnouts() ;
    Xnet.setup( Loco28, max45dir ) ; // N.B. may need to change depending on what multimaus will do.

    for( int i = 0 ; i < 8 ; i ++ ) { pinMode( relay[i], OUTPUT ) ;         // set all relay pins on output
                                      digitalWrite( relay[i], HIGH ) ; }    // relay module work inverted

    pinMode( ledPin, OUTPUT ) ;
    throttle.begin() ;

    Xnet.setPower( csNormal ) ;
}

void loop()
{
    Xnet.update() ;
    throttle.update() ;

    updateSpeed() ;
    shortCircuit() ;

    turnOffServo() ;

    layRoutes() ;
}