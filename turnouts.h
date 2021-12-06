#include <Arduino.h> 

#define nTurnouts 16

extern void setTurnout( uint8_t, uint8_t ) ;
extern void initTurnouts() ;
extern void turnOffServo() ;
