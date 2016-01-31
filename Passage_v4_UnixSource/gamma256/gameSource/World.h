#include <stdint.h>
typedef uint32_t Uint32;


// these can be called once at beginning and end of app execution
// since loaded graphics can be reused for multiple games
void loadWorldGraphics();
void destroyWorldGraphics();
    

// these should be called at the beginning and end of each new game
void initWorld();
void destroyWorld();
        

Uint32 sampleFromWorld( int inX, int inY, double inWeight = 1.0 );


void startPrizeAnimation( int inX, int inY );
void startDustAnimation( int inX, int inY );

void setPlayerPosition( int inX, int inY );
void setPlayerSpriteFrame( int inFrame );


void getSpousePosition( int *outX, int *outY );

char haveMetSpouse();

void meetSpouse();

void startHeartAnimation( int inX, int inY );

void diePlayer();

void dieSpouse();

char isSpouseDead();

char isPlayerDead();



// age in range 0..1
void setCharacterAges( double inAge );




// push animations forward one step
void stepAnimations();




int getTileWidth();


int getTileHeight();

