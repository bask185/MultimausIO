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
#define C   1   // curved
#define S   0   // straight
#define XX  0   // dont care
const int routes[17][7] =
{//        points              |  relays
    { 1|C, 2|C, 3|S,  XX,  XX,    1,  3 } ,
    { 1|C, 2|C, 3|C,  XX,  XX,    1, XX } ,
    { 1|C, 2|S, 4|S,  XX,  XX,    1, XX } ,
    { 1|C, 2|S, 4|S, 6|S, 7|S,    1,  5 } ,
    { 1|C, 2|S, 4|S, 6|C,  XX,    1,  6 } ,
    { 1|S, 2|C, 3|S,  XX,  XX,   XX,  3 } ,
    { 1|S, 2|C, 3|C,  XX,  XX,   XX, XX } ,
    { 1|S, 2|S, 4|S,  XX,  XX,   XX, XX } ,
    { 1|S, 2|S, 4|S, 6|S, 7|S,   XX,  5 } ,
    { 1|S, 2|S, 4|S, 6|C,  XX,   XX,  6 } ,
    { 5|C, 4|C,  XX,  XX,  XX,    2, XX } ,
    { 5|S,  XX,  XX,  XX,  XX,    2,  4 } ,
    { 5|C, 4|C, 6|S, 7|S,  XX,    2,  5 } ,
    { 5|C, 4|C, 6|C,  XX,  XX,    2,  6 } ,
    { 7|C,  XX,  XX,  XX,  XX,   XX,  5 } ,
    { 6|S, 7|S,  XX,  XX,  XX,   XX,  5 } ,
    { 6|C,  XX,  XX,  XX,  XX,   XX,  6 } ,
}
#undef  C
#undef  S
#undef  XX