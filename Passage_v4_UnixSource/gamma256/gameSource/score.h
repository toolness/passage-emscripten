#include <stdint.h>
typedef uint32_t Uint32;



void initScore();



int getScoreHeight();



// draws score in upper-right corner of image
void drawScore( Uint32 *inImage, int inWidth, int inHeight, int inScore );



void destroyScore();
