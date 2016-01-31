#include "musicPlayer.h"

#include "minorGems/system/Thread.h"

#include <SDL/SDL.h>


extern void loadMusicImage( char *inTGAFileName );

extern void audioCallback( void *inUserData, Uint8 *inStream, 
                           int inLengthToFill );


int main() {
    // don't actually start SDL loop, because profiler can't pick
    // up callbacks (why not?)

    loadMusicImage( "music.tga" );

    int length = 512;
    
    Uint8 *fakeStream = new Uint8[ length ];
    

    for( int i=0; i<20000; i++ ) {
        
        int x = 5 + 1;
        x ++;
        
        audioCallback( NULL, fakeStream, length );
        }
    }
