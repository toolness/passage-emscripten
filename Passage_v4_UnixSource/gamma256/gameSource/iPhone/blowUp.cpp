#include "blowUp.h"

#include <string.h>



void blowupOntoScreen( Uint32 *inImage, int inWidth, int inHeight,
					  int inBlowFactor, Uint32 *inScreenPixels, 
                      int inScreenWidth, int inScreenHeight ) {
    
    int newWidth = inBlowFactor * inWidth;
    int newHeight =  inBlowFactor * inHeight;
    
    int yOffset = ( inScreenHeight - newHeight ) / 2;
    
    // "up" a bit more on iPhone to make room for touch widget
    int xOffset = 2 * ( inScreenWidth - newWidth ) / 3;
    
	// pitch equals width for iPhone implementation
    int scanlineWidth = inScreenWidth;
    
    Uint32 *pixels = inScreenPixels;
    

    // looping across the smaller image, instead of across the larger screen,
    // was discovered using the profiler.


    // an entire screen row is repeated inBlowFactor times down the screen
    // (as a row of pixel boxes)
    // Thus, we can offload a lot more work onto memcpy if we assemble one
    // of these rows and then memcpy it onto the screen inBlowFactor times
    for( int y=0; y<inHeight; y++ ) {
        
        Uint32 *screenRow = new Uint32[ newWidth ];
        
        // flip image vertically for iPhone
        int gameY = (inHeight - 1) - y;
        
        // fill the screen row with rows from the pixel boxes
        for( int x=0; x<inWidth; x++ ) {

            Uint32 pixelValue = inImage[ gameY * inWidth + x ];            
            
            // swap red and blue on iPhone
            unsigned char red = (pixelValue >> 16) & 0xFF;
            unsigned char green = (pixelValue >> 8) & 0xFF;
            unsigned char blue = (pixelValue >> 0) & 0xFF;
            
            pixelValue = blue << 16 | green << 8 | red;
            
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
