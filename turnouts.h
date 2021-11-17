#include <Arduino.h> 

#define nTurnouts 8

typedef struct Turnouts {
	uint8_t lowPos ; 
	uint8_t highPos ;
	uint8_t state ;
} ;

extern void setTurnout( uint8_t, uint8_t ) ;
extern void initTurnouts( ) ;
