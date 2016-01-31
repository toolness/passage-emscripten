#include "drawIntoScreen.h"


/**
 * Blows an image up onto the screen using nearest neighbor (pixelated)
 * interpolation.
 */
void blowupOntoScreen( Uint32 *inImage, int inWidth, int inHeight,
                       int inBlowFactor, Uint32 *inScreenPixels, 
                       int inScreenWidth, int inScreenHeight );
