// interface for generic screen drawing framework

#include <stdint.h>

typedef uint32_t Uint32;


void initScreenDrawer( Uint32 *inScreenBuffer, int inWidth, int inHeight );

// each pixel is 4 characters
void drawIntoScreen( Uint32 *inScreenBuffer, int inWidth, int inHeight );

// set device orientation from accelerometer
void setOrientation( float inX, float inY );

void touchStartPoint( float inX, float inY );

void touchMovePoint( float inX, float inY );

void touchEndPoint( float inX, float inY );

void freeScreenDrawer();
