uint8_t getRouteIndex( uint8_t first, uint8_t second)
{
    switch ( first ) {
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

        
        
        
        


  /*  
1).  1 -> 4
2).  1 -> 5
3).  1 -> 6
4).  1 -> 8
5).  1 -> 9

6).  2 -> 4
7).  2 -> 5
8).  2 -> 6
9).  2 -> 8
10). 2 -> 9

11). 3 -> 6
12). 3 -> 7
13). 3 -> 8
14). 3 -> 9

15). 5 -> 8

16). 6 -> 8
17). 6 -> 9
*/
}