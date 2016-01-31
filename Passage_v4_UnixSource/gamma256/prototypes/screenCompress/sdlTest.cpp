// FIXME:
// why does this end up with a blank, white screen after 5 seconds
// while the demo code (testsprite) works fine?
// Does it only happen on win32



#include <math.h>
#include <signal.h>
#include <stdlib.h>

// for memcpy
#include <string.h>

#include <signal.h>
#include <stdio.h>


void catch_int(int sig_num) {
	printf( "Quiting...\n" );
	//currentStep = numSteps;
	exit( 0 );
	signal( SIGINT, catch_int );
	}


// if SDL.h is included before our main function, the linker complains
// about missing WinMain
// This fixes it
int mainFunction();

int main() {
    mainFunction();
    }


#include <SDL/SDL.h>


int mainFunction() {


    // let catch_int handle interrupt (^c)
    signal( SIGINT, catch_int );

    int w = 640;
    int h = 480;

    if( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_NOPARACHUTE ) < 0 ) {
        printf( "Couldn't initialize SDL: %s\n", SDL_GetError() );
        return -1;
        }
    
    SDL_Surface *screen = SDL_SetVideoMode( w, h, 
                                            32, 
                                            SDL_SWSURFACE );
    
    if ( screen == NULL ) {
        printf( "Couldn't set %dx%dx32 video mode: %s\n", w, h,
            SDL_GetError() );
        return-1;
        }

    Uint32 *buffer = (Uint32 *)( screen->pixels );

    char white = false;

    char done =  false;
    
    while( !done ) {
        if( white ) {
            SDL_FillRect(screen, NULL, 0x00FFFFFF);
            }
        else {
            SDL_FillRect(screen, NULL, 0x00000000);
            }
        /*
        for( int y=0; y<h; y++ ) {
            for( int x=0; x<w; x++ ) {
                            
                    
                if( white ) {
                    buffer[ y * w + x ] = 0x00FFFFFF;
                    }
                else {
                    buffer[ y * w + x ] = 0x00000000;
                    }
             
                }
            }
        */
        white = ! white;
        //SDL_UpdateRect( screen, 0, 0, w, h );
        SDL_Flip( screen );	
        
        // check for events to quit
        SDL_Event event;
        while ( SDL_PollEvent(&event) ) {
            switch (event.type) {
                case SDL_KEYDOWN:
                    /* Any keypress quits the app... */
                case SDL_QUIT:
                    done = true;
                    break;
                default:
                    break;
                }
            }
        }
    }
