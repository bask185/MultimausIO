#include "routes.h"
#include "macros.h"
#include "io.h"


extern void setOutput( uint8_t Address, uint8_t functions ) ;
extern void setTurnout( uint8_t address, uint8_t state ) ; 


enum routeStates
{
    freed,
    setting,
    set,
} ;

uint8_t counter,  selectedRoute, route = freed  ;
const int elementsPerRoute  =    7 ;
const int C                 = 0x80 ;                                            //    curved
const int S                 = 0x00 ;                                            //  straight
const int X                 = 0xFF ;                                            // dont care



void freeRoute()
{
    if( route != set ) return ;                                                 // route can be freed only after it is is set.
    
    for( int i = 0 ; i < 8 ; i ++ )
    {
        digitalWrite( relay[i], 1 ) ;                                           // turn off relays
    }
    
    route = freed ;
}

uint8_t  getNewRoute ( uint8_t first, uint8_t second )
{
    if( second < first )                                                        // route 1 <> 9 is the same as route 9 <> 1, 
    {                                                                           // therefor I use XOR ensure that the first number is the lowest
        first  ^= second ;
        second ^=  first ;
        first  ^= second ;
    }

    switch ( first ) {

    // first      // second                 //  selectedRoute 
    case 1: switch( second ) { case 4 : return  1 ;
                               case 5 : return  2 ;
                               case 6 : return  3 ;
                               case 8 : return  4 ;
                               case 9 : return  5 ; }
    case 2: switch( second ) { case 4 : return  6 ;
                               case 5 : return  7 ;
                               case 6 : return  8 ;
                               case 8 : return  9 ;
                               case 9 : return 10 ; }
    case 3: switch( second ) { case 6 : return 11 ;
                               case 7 : return 12 ;
                               case 8 : return 13 ;
                               case 9 : return 14 ; }
    case 5: switch( second ) { case 8 : return 15 ; }
    case 6: switch( second ) { case 8 : return 16 ;
                               case 9 : return 17 ; }
    }
}

/*   8x8 possible routes in theory.
(x) = relay
 x  = point
[x] = track definition

                      ,----(3)-- [4]
  [1]---(1)----.    ,3----[5]------.
[2]-------------1--2-4----[6]---- 6 7 -------(5) [8]
  [3]---(2)---------5-----(4)-[7]  `---------(6) [9]

in practice only these are possible

                            POINTS        |   RELAY
                 1    2    3    4    5    | 
1).  1 -> 4     1|C, 2|C, 3|S,  XX,  XX,    1,  3
2).  1 -> 5     1|C, 2|C, 3|C,  XX,  XX,    1, XX
3).  1 -> 6     1|C, 2|S, 4|S,  XX,  XX,    1, XX
4).  1 -> 8     1|C, 2|S, 4|S, 6|S, 7|S,    1,  5
5).  1 -> 9     1|C, 2|S, 4|S, 6|C,  XX,    1,  6
6).  2 -> 4     1|S, 2|C, 3|S,  XX,  XX,   XX,  3    
7).  2 -> 5     1|S, 2|C, 3|C,  XX,  XX,   XX, XX
8).  2 -> 6     1|S, 2|S, 4|S,  XX,  XX,   XX, XX
9).  2 -> 8     1|S, 2|S, 4|S, 6|S, 7|S,   XX,  5
10). 2 -> 9     1|S, 2|S, 4|S, 6|C,  XX,   XX,  6
11). 3 -> 6     5|C, 4|C,  XX,  XX,  XX,    2, XX
12). 3 -> 7     5|S,  XX,  XX,  XX,  XX,    2,  4
13). 3 -> 8     5|C, 4|C, 6|S, 7|S,  XX,    2,  5
14). 3 -> 9     5|C, 4|C, 6|C,  XX,  XX,    2,  6
15). 5 -> 8     7|C,  XX,  XX,  XX,  XX,   XX,  5 
16). 6 -> 8     6|S, 7|S,  XX,  XX,  XX,   XX,  5 
17). 6 -> 9     6|C,  XX,  XX,  XX,  XX,   XX,  6 

There are only 17 routes possible, the routes with the most items has 7 outputs. 
Without compression, we need atleast 17x7 = 119 bytes in EEPROM or FLASH
*/

const int routes[17][7] =
{//        points              |  relays
    { 1|C, 2|C, 3|S,   X,   X,    1,  3 } ,
    { 1|C, 2|C, 3|C,   X,   X,    1,  X } ,
    { 1|C, 2|S, 4|S,   X,   X,    1,  X } ,
    { 1|C, 2|S, 4|S, 6|S, 7|S,    1,  5 } ,
    { 1|C, 2|S, 4|S, 6|C,   X,    1,  6 } ,
    { 1|S, 2|C, 3|S,   X,   X,    X,  3 } ,
    { 1|S, 2|C, 3|C,   X,   X,    X,  X } ,
    { 1|S, 2|S, 4|S,   X,   X,    X,  X } ,
    { 1|S, 2|S, 4|S, 6|S, 7|S,    X,  5 } ,
    { 1|S, 2|S, 4|S, 6|C,   X,    X,  6 } ,
    { 5|C, 4|C,   X,   X,   X,    2,  X } ,
    { 5|S,   X,   X,   X,   X,    2,  4 } ,
    { 5|C, 4|C, 6|S, 7|S,   X,    2,  5 } ,
    { 5|C, 4|C, 6|C,   X,   X,    2,  6 } ,
    { 7|C,   X,   X,   X,   X,    X,  5 } ,
    { 6|S, 7|S,   X,   X,   X,    X,  5 } ,
    { 6|C,   X,   X,   X,   X,    X,  6 } ,
} ;
  

void layRoutes()
{
    if( route != setting ) return ;

    REPEAT_MS( 350 )
    {
        uint8_t address = X ;
        bool    state   ;

        while( address == X )                                                   // keep looping until valid address is found
        {
            address = (routes[ selectedRoute ][counter]  & 0x7F) - 1 ;          // fetch new address
            state   =  routes[ selectedRoute ][counter] >> 7 ;

            if( ++ counter >= elementsPerRoute )                                // if all elements are set -> return
            {
                route = set ;
                return ;
            }
        }

        if( counter <= 4 )  setTurnout( address, state ) ;                      // 0-4 -> any of the 5 points
        else                digitalWrite( relay[address], state ) ;             // 5-6 -> any of the 2 relays

    } END_REPEAT
    
    counter ++ ;
}

void setRoute( uint8_t track )
{
    static uint8_t firstButton, secondButton ;
    
    for( int i = 0 ; i < 8 ; i ++ )
    {
        digitalWrite( relay[i], track >> i ) ;                                  // turn off relays
    }
    
    if( route != freed ) return ;                                               // if route is not free, do not set a new one

    if( firstButton  == 0xFF ) firstButton  = track ;
    if( secondButton == 0xFF ) secondButton = track ;

    if( firstButton != 0xFF && secondButton != 0xFF )
    {
        secondButton = firstButton = 0xFF ;

        selectedRoute  =  getNewRoute ( firstButton, secondButton ) - 1 ;
        counter = 0 ;                                                           // trigger the function "layRoutes" to start laying in the route

        route = setting ;
    }
}