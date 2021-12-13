#include <EEPROM.h>
#include "turnouts.h"
#include "io.h"
#include "Adafruit_PWMServoDriver.h"

#define INIT_ADDR       0X00
#define ADDR_S1         0X01
#define ADDR_S2         0X03
#define ADDR_S3         0X05
#define ADDR_S4         0X07
#define ADDR_S5         0X09
#define ADDR_S6         0X0B
#define ADDR_S7         0X0D
#define ADDR_S8         0X0F

Adafruit_PWMServoDriver servoDriver ;// = Adafruit_PWMServoDriver();

#define nTurnouts 16

typedef struct Turnouts {
	uint8_t lowPos ; 
	uint8_t highPos ;
	uint8_t state ;
} ;
Turnouts turnout[ nTurnouts ];

static uint32_t prevTime ;
static uint8_t lastServo ;

void initTurnouts() 
{
    servoDriver.begin() ;
    servoDriver.reset() ;
    servoDriver.setOscillatorFrequency( 27000000 ) ;
    servoDriver.setPWMFreq( 50 ) ;  // Analog servos run at ~50 Hz updates

    if( EEPROM.read( INIT_ADDR ) == 255)
    {
         EEPROM.write( INIT_ADDR, 0xCC ) ;

        for( int i = 0 ; i < 8 ; i ++ ) 
        {
            EEPROM.write( ADDR_S1 + ( 2 * i )     , 85 ) ;
            EEPROM.write( ADDR_S1 + ( 2 * i ) + 1 , 95 ) ;
        }
    }

    for( int i = 0 ; i < 8 ; i ++ ) 
    {
         turnout[i].lowPos  = EEPROM.read( ADDR_S1 + ( 2 * i )     ) ;
         turnout[i].highPos = EEPROM.read( ADDR_S1 + ( 2 * i ) + 1 ) ;
    }
}


void setTurnout( uint8_t ID, uint8_t state )
{    
    if( ID <= nTurnouts ) {    // ignores non existing turnouts

        turnout[ID].state = state ;

        uint8_t degrees ;
        
        if( state ) degrees = turnout[ID].highPos;
        else        degrees = turnout[ID].lowPos;

        degrees = constrain( degrees, 20, 160 ) ;

        uint16_t us = map( degrees, 0, 180, 204, 408 );             // map degrees to pulse lengths, numbers don't make sense but it works
        servoDriver.setPWM( ID, 0, us );

        lastServo = ID ;
        prevTime = millis() ;                                       // set timeout to cut off servo's
    }
}


/**
 * @brief Adjust every servo position real time and pushes change to EEPROM.
 * 
 * @param F11_F12 either -3 or +3.
 */
void adjustServo( int8_t F11_F12 )
{
    uint16_t eeAddress ;

    if( turnout[lastServo].state == 0 )                 // ADJUST EITHER LOW POSITION...
    {
        eeAddress = ADDR_S1 + (lastServo * 2) ;
        turnout[lastServo].lowPos  += F11_F12 ;          // add or substract 
        turnout[lastServo].lowPos = constrain( turnout[lastServo].lowPos, 20, 160 ) ;
        setTurnout( lastServo, 0 ) ;                    // update servo
        EEPROM.write(eeAddress, turnout[lastServo].lowPos ) ;         
    }
    else
    {                                                   // ... OR HIGH POSITIONS
        eeAddress = ADDR_S1 + (lastServo * 2) + 1;
        turnout[lastServo].highPos += F11_F12 ; 
        turnout[lastServo].highPos = constrain( turnout[lastServo].highPos, 20, 160 ) ;
        setTurnout( lastServo, 1 ) ;
        EEPROM.write( eeAddress , turnout[lastServo].highPos ) ; 
    }
}

void turnOffServo()
{
    uint32_t currentTime = millis() ;

    if( currentTime - prevTime >= 350 )      // servo gets 0.2s to move
    {
        prevTime = currentTime ; 
        servoDriver.setPin( lastServo , 0, 0 ) ;
    }

}

void whipeEEPROM()
{
    EEPROM.write( INIT_ADDR, 255 ) ;       //for update
}