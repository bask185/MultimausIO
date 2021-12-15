#include "routes.h"

uint8_t counter, index ;

uint8_t getRouteIndex( uint8_t first, uint8_t second )
{
    if( second < first )      // route 1 <> 9 is the same as route 9 <> 1, 
    {                         // therefor I use XOR ensure that the first number is the lowest
        first  ^= second ;
        second ^=  first ;
        first  ^= second ;
    }

    switch ( first ) {

    // first      // second                 // index
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

There are only 17 routes possible, the routes with the most items has 7 outputs. Without compression, we need atleast 17x7 = 119 bytes in EEPROM or FLASH
*/
#define C   0x80    // curved
#define S   0x00    // straight
#define X   0x00    // dont care
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
}
#undef  C
#undef  S
#undef  X        

void layRoutes()
{
    if( counter == 8 ) return ;
    uint8_t address = routes[index][counter]  & 0x7F ;
    bool    state   = routes[index][counter] >>    7 ;
    
    
    
    counter ++ ;
}

void setRoute( uint8_t route )
{
    static uint8_t firstButton, secondButton ;

    if( firstButton  == 0xFF ) firstButton = route ;
    if( secondButton == 0xFF ) secondButton = route ;

    if( firstButton != 0xFF && secondButton != 0xFF )
    {
        secondButton = firstButton = 0xFF ;

        index = getRouteIndex( firstButton, secondButton) - 1 ;
        counter = 0 ;
    }
}