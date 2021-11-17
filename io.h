/*
List of IO
There are 16 servos and 8 relays to be controlled. so 24 states to be remembered

Servo   1 -  8, will be on address 1, F1 - F8
Servo   9 - 16, will be on address 2, F1 - F8
Relay   1 -  8, will be on address 3, F1 - F8


*/

#define relayPin1 3
#define relayPin2 4
#define relayPin3 5
#define relayPin4 6
#define relayPin5 7
#define relayPin6 8
#define relayPin7 9
#define relayPin8 10

#define dirPin1 11
#define dirPin2 11
#define enablePin A0

#define CS A6

#define dirPin 2