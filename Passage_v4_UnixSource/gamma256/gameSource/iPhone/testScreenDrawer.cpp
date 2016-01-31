

#include "drawIntoScreen.h"

#include <stdlib.h>

void drawIntoScreen( Uint32 *inScreenBuffer, int inWidth, int inHeight ) {
	// add dot at random spot
	int x = rand() % inWidth;
	int y = rand() % inHeight;
	
	int i = y * inWidth + x;
	
	inScreenBuffer[ i ] = 0xFFFFFFFF;
}

void initScreenDrawer( Uint32 *inScreenBuffer, int inWidth, int inHeight ) {
}

void freeScreenDrawer() {
}