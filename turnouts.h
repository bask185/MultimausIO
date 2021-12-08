#include <Arduino.h> 

#define nTurnouts 16

const int F9  = -3 ; // degrees at the time
const int F10 =  3 ;

extern void setTurnout( uint8_t, uint8_t ) ;
extern void initTurnouts() ;
extern void turnOffServo() ;
void adjustServo( int8_t ) ;
void whipeEEPROM() ;
