/*
List of IO
There are 16 servos and 8 relays to be controlled. so 24 states to be remembered

Servo   1 -  8, will be on address 1, F1 - F8
Servo   9 - 16, will be on address 2, F1 - F8
Relay   1 -  8, will be on address 3, F1 - F8


*/

const int relayPin1     = 3 ;
const int relayPin2     = 4 ;
const int relayPin3     = 5 ;
const int relayPin4     = 6 ;
const int relayPin5     = 7 ;
const int relayPin6     = 8 ;
const int relayPin7     = 9 ;
const int relayPin8     = 10 ;
const int dirPin1       = 11 ;
const int dirPin2       = 11 ;
const int enablePin     = A0 ;
const int CS            = A6 ;
const int max45dir      = 2 ;

const uint8_t relay[] = 
{
    relayPin1,
    relayPin2,
    relayPin3,
    relayPin4,
    relayPin5,
    relayPin6,
    relayPin7,
    relayPin8
} ;