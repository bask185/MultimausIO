#include "turnouts.h"
#include "Adafruit_PWMServoDriver.h"

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
    
    turnout[0].lowPos = 106 ; turnout[0].highPos =  74 ; turnout[0].state = 1 ; // 1
    turnout[1].lowPos =  74 ; turnout[1].highPos = 106 ; turnout[1].state = 1 ; // 2
    turnout[2].lowPos =  74 ; turnout[2].highPos = 106 ; turnout[2].state = 1 ; // 3
    turnout[3].lowPos = 106 ; turnout[3].highPos =  74 ; turnout[3].state = 1 ; // 4
    turnout[4].lowPos =  74 ; turnout[4].highPos = 106 ; turnout[4].state = 1 ; // 5
    turnout[5].lowPos =  74 ; turnout[5].highPos = 106 ; turnout[5].state = 1 ; // 6
    turnout[6].lowPos = 106 ; turnout[6].highPos =  74 ; turnout[6].state = 1 ; // 7

    // for(byte j = 1 ; j < 8 ; j ++ ) 
    // {
    //     setTurnout( j, 0 ) ;
    //     delay(200);
    // }
}


void setTurnout( uint8_t ID, uint8_t state ) {    

    if( ID > 0 ) ID -- ;     // I screwed up with 0 index system...

    if( ID <= nTurnouts ) {    // ignores non existing turnouts

        turnout[ID].state = state ;

        uint8_t degrees ;
        
        if( state ) degrees = turnout[ID].highPos;
        else        degrees = turnout[ID].lowPos;

        uint16_t us = map( degrees, 0, 180, 204, 408 );             // map degrees to pulse lengths, numbers don't make sense but it works
        servoDriver.setPWM( ID, 0, us );

        lastServo = ID ;
        prevTime = millis() ;                                 // set timeout to cut off servo's
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