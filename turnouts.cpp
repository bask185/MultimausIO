#include <EEPROM.h>
#include "turnouts.h"
#include "Adafruit_PWMServoDriver.h"

#define INIT_ADDR       0X00
#define ADDR_S1         0X01
#define ADDR_S2         0X03
#define ADDR_S3         0X04
#define ADDR_S4         0X05
#define ADDR_S5         0X07
#define ADDR_S6         0X09
#define ADDR_S7         0X0B
#define ADDR_S8         0X0D

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

    // digitalWrite(13, HIGH) ;
    // servoDriver.sleep() ;
    // delay(1000);
    // digitalWrite(13, LOW) ;
    // servoDriver.wakeup();
    // servoDriver.reset() ;
    // delay(1000);

    if(INIT_ADDR == 255)
    {
        EEPROM.write( INIT_ADDR, 0xCC   ) ; // some random value to make it not 255 :-D
        EEPROM.write( ADDR_S1     , 106 ) ; 
        EEPROM.write( ADDR_S1 + 1 ,  74 ) ; 
        EEPROM.write( ADDR_S2     ,  74 ) ;
        EEPROM.write( ADDR_S2 + 1 , 106 ) ; 
        EEPROM.write( ADDR_S3     ,  74 ) ; 
        EEPROM.write( ADDR_S3 + 1 , 106 ) ; 
        EEPROM.write( ADDR_S4     , 106 ) ; 
        EEPROM.write( ADDR_S4 + 1 ,  74 ) ; 
        EEPROM.write( ADDR_S5     ,  74 ) ; 
        EEPROM.write( ADDR_S5 + 1 , 106 ) ; 
        EEPROM.write( ADDR_S6     ,  74 ) ; 
        EEPROM.write( ADDR_S6 + 1 , 106 ) ; 
        EEPROM.write( ADDR_S7     , 106 ) ; 
        EEPROM.write( ADDR_S7 + 1 ,  74 ) ; 
        EEPROM.write( ADDR_S8     , 106 ) ;  // not in use
        EEPROM.write( ADDR_S8 + 1 ,  74 ) ; 
    }
    
    turnout[0].lowPos = EEPROM.read( ADDR_S1 ) ; turnout[0].highPos = EEPROM.read( ADDR_S1 + 1 ) ; // turnout[0].state = 1 ; // 1 N.B I should be stuffed in a for-loop later on
    turnout[1].lowPos = EEPROM.read( ADDR_S2 ) ; turnout[1].highPos = EEPROM.read( ADDR_S2 + 1 ) ; // turnout[1].state = 1 ; // 2
    turnout[2].lowPos = EEPROM.read( ADDR_S3 ) ; turnout[2].highPos = EEPROM.read( ADDR_S3 + 1 ) ; // turnout[2].state = 1 ; // 3
    turnout[3].lowPos = EEPROM.read( ADDR_S4 ) ; turnout[3].highPos = EEPROM.read( ADDR_S4 + 1 ) ; // turnout[3].state = 1 ; // 4
    turnout[4].lowPos = EEPROM.read( ADDR_S5 ) ; turnout[4].highPos = EEPROM.read( ADDR_S5 + 1 ) ; // turnout[4].state = 1 ; // 5
    turnout[5].lowPos = EEPROM.read( ADDR_S6 ) ; turnout[5].highPos = EEPROM.read( ADDR_S6 + 1 ) ; // turnout[5].state = 1 ; // 6
    turnout[6].lowPos = EEPROM.read( ADDR_S7 ) ; turnout[6].highPos = EEPROM.read( ADDR_S7 + 1 ) ; // turnout[6].state = 1 ; // 7
    turnout[7].lowPos = EEPROM.read( ADDR_S8 ) ; turnout[7].highPos = EEPROM.read( ADDR_S8 + 1 ) ; // turnout[7].state = 1 ; // 8

    // for(byte j = 1 ; j < 8 ; j ++ ) 
    // {
    //     setTurnout( j, 0 ) ;
    //     delay(200);
    // }
}


void setTurnout( uint8_t ID, uint8_t state )
{    
    if( ID <= nTurnouts ) {    // ignores non existing turnouts

        turnout[ID].state = state ;

        uint8_t degrees ;
        
        if( state ) degrees = turnout[ID].highPos;
        else        degrees = turnout[ID].lowPos;

        uint16_t us = map( degrees, 0, 180, 204, 408 );             // map degrees to pulse lengths, numbers don't make sense but it works
        servoDriver.setPWM( ID, 0, us );

        lastServo = ID ;
        prevTime = millis() ;                                       // set timeout to cut off servo's
    }
}


/**
 * @brief Adjust every servo position real time and pushes change to EEPROM.
 * 
 * @param F9_F10 either -1 or +1.
 */
void adjustServo( int8_t F9_F10 )
{
    if( turnout[lastServo].state == 0 )
    {
        turnout[lastServo].lowPos  += F9_F10 ;          // add or substract 1
        setTurnout( lastServo, 0 ) ;                    // update servo
        EEPROM.write( ADDR_S1 + (lastServo * 2)  ,      // commit change to EEPROM
                      turnout[lastServo].highPos ) ; 
    }
    else
    {
        turnout[lastServo].highPos += F9_F10 ; 
        setTurnout( lastServo, 1 ) ;
        EEPROM.write( ADDR_S1 + (lastServo * 2) + 1 , // one extra offset for the high pos address;
                      turnout[lastServo].highPos ) ; 
    }
}

void turnOffServo()
{
    uint32_t currentTime = millis() ;

    if( currentTime - prevTime >= 200 )      // servo gets 0.2s to move
    {
        prevTime = currentTime ; 
        servoDriver.setPin( lastServo , 0, 0 ) ;
    }

}