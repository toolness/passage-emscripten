#include "landscape.h"

#include <stdio.h>

void printBinary( unsigned char inVal ) {
    for( int b=7; b>=0; b-- ) {
        if( ( inVal >> b ) & 0x01 == 1 ) {
            printf( "1" );
            }
        else {
            printf( "0" );
            }
        }
    }

int main() {
    
    int numAllZeroOrig = 0;
    int numAllZeroNew = 0;

    for( int i=0; i<20; i++ ) {
        double value = noise3d( 37462 * i, 1747 * i, i * 3748147 );
        
        

        printf( "%d ... %f .... ", i, value );


        
        unsigned char castedChar = (unsigned char)( value * 15 );

        unsigned char chestVal = (unsigned char)( value * 15 ) & 0x3F;
        
        // show in binary        
        printf( "casted char = %d ... original val = ", castedChar );
        printBinary( chestVal );
        
        printf( "\n" );
        
        /*        
        if( chestVal == 0 ) {
            numAllZeroOrig ++;
            }

        value += 1;
        value /= 2;

        unsigned char newVal = (unsigned char)( value * 63 ) & 0x3F;

        printf( "\n ... new value = %f ....  ", value );
        printBinary( newVal );
        printf( "\n\n" );

        if( newVal == 0 ) {
            numAllZeroNew ++;
            }
        */
        
        
        }

    printf( "Num all zero, orig = %d, new = %d \n", 
            numAllZeroOrig, numAllZeroNew );

    return 0;
    }
