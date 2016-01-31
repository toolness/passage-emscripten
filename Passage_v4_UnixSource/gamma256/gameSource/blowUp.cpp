#include "blowUp.h"

#include <string.h>



void blowupOntoScreen( Uint32 *inImage, int inWidth, int inHeight,
                       int inBlowFactor, SDL_Surface *inScreen ) {
    
    int newWidth = inBlowFactor * inWidth;
    int newHeight =  inBlowFactor * inHeight;
    
    int yOffset = ( inScreen->h - newHeight ) / 2;
    int xOffset = ( inScreen->w - newWidth ) / 2;
    
    // pitch is in bytes
    // convert to width in pixels
    int scanlineWidth = inScreen->pitch / 4;
    
    Uint32 *pixels = (Uint32 *)( inScreen->pixels );
    

    // looping across the smaller image, instead of across the larger screen,
    // was discovered using the profiler.


    // an entire screen row is repeated inBlowFactor times down the screen
    // (as a row of pixel boxes)
    // Thus, we can offload a lot more work onto memcpy if we assemble one
    // of these rows and then memcpy it onto the screen inBlowFactor times
    for( int y=0; y<inHeight; y++ ) {
        
        Uint32 *screenRow = new Uint32[ newWidth ];
        
        // fill the screen row with rows from the pixel boxes
        for( int x=0; x<inWidth; x++ ) {

            Uint32 pixelValue = inImage[ y * inWidth + x ];
            
            // spread this pixel across an inBlowFactor-wide box row in
            // the screen row
            
            int boxXStart = inBlowFactor * x;
            int boxXEnd = boxXStart + inBlowFactor;
            
            // make an array to represent one row of this box
            // we can thus replace the inner loop with a memcpy below
            for( int i=boxXStart; i<boxXEnd; i++ ) {
                screenRow[i] = pixelValue;
                }
            }

        // now copy the row onto the screen inBlowFactor times

        int screenRowStart = ( yOffset + y * inBlowFactor ) * scanlineWidth 
            + xOffset;
        int screenRowEnd = screenRowStart + inBlowFactor * scanlineWidth;
        int screenRowIncrement = scanlineWidth;
        
        for( int screenRowIndex = screenRowStart; 
             screenRowIndex < screenRowEnd;
             screenRowIndex += screenRowIncrement ) {
            
	    memcpy( &( pixels[ screenRowIndex ] ),
                    screenRow, newWidth * 4 );
            }
        
        delete [] screenRow;
        
        }
            
    }
