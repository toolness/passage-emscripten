#include "score.h"
#include "common.h"


#include "minorGems/util/stringUtils.h"
#include "minorGems/graphics/Image.h"



Image *numeralImage;
int numeralW = 3;
int numeralH = 4;

int imagePixelCount;

double *numeralRed;
double *numeralGreen;
double *numeralBlue;

Uint32 *numeralARGB;



void initScore() {
    numeralImage =  readTGA( "numerals.tga" );
    
    imagePixelCount = numeralImage->getWidth() * numeralImage->getHeight();
    
    numeralRed =  new double[ imagePixelCount ];
    numeralGreen =  new double[ imagePixelCount ];
    numeralBlue =  new double[ imagePixelCount ];
    
    numeralARGB = new Uint32[ imagePixelCount ];
    
    for( int i=0; i<imagePixelCount; i++ ) {
        numeralRed[i] = 255 * numeralImage->getChannel(0)[ i ];
        numeralGreen[i] = 255 * numeralImage->getChannel(1)[ i ];
        numeralBlue[i] = 255 * numeralImage->getChannel(2)[ i ];
        
        unsigned char r = 
            (unsigned char)( 
                numeralRed[ i ] );
    
        unsigned char g = 
            (unsigned char)( 
                numeralGreen[ i ] );
        
        unsigned char b = 
            (unsigned char)( 
                numeralBlue[ i ] );
            
        numeralARGB[i] = r << 16 | g << 8 | b;
        }
    }



int getScoreHeight() {
    return numeralH;
    }



void drawScore( Uint32 *inImage, int inWidth, int inHeight, int inScore ) {
    
    char *scoreString = autoSprintf( "%d", inScore );
    
    int numDigits = strlen( scoreString );
    
    int xPosition = inWidth - numDigits * ( numeralW + 1 );
    
    for( int i=0; i<numDigits; i++ ) {
        char digit = scoreString[i];
        
        int digitNumber = digit - '0';
        
        for( int y=0; y<numeralH; y++ ) {
            int numeralY = y + digitNumber * ( numeralH + 1 );
            
            for( int x=0; x<numeralW; x++ ) {
                int imageX = x + xPosition;
                
                // copy pixels
                inImage[ y * inWidth + imageX ] =
                    numeralARGB[ numeralY * numeralW + x ];
                }
            }
        
        xPosition += numeralW + 1;
        }
    

    delete [] scoreString;
    

    }



void destroyScore() {
    delete numeralImage;
    
    
    delete [] numeralRed;
    delete [] numeralGreen;
    delete [] numeralBlue;

    delete [] numeralARGB;
    
    }
