/*
 * Modification History
 *
 * 2007-September-25   Jason Rohrer
 * Created.  
 */

#include "landscape.h"

#include "minorGems/graphics/Image.h"
#include "minorGems/graphics/converters/TGAImageConverter.h"

#include "minorGems/io/file/File.h"

#include "minorGems/io/file/FileInputStream.h"

// #include "minorGems/system/Thread.h"


#include <math.h>
#include <signal.h>
#include <stdlib.h>
#include <time.h>


// for memcpy
#include <string.h>



// size of game image
int width = 100;
int height = 16;

// size of screen
int screenWidth = 640;
int screenHeight = 480;

// blow-up factor when projecting game onto screen
int blowUpFactor = 6;
//int blowUpFactor = 1;


char fullScreen = false;

double timeDelta = -0.1;
//double timeDelta = -0.0;


int mainFunction();



// must do this to prevent WinMain linker errors on win32
int main() {
    mainFunction();
    }



#include <SDL/SDL.h>


// the joystick to read from, or NULL if there's no available joystick
SDL_Joystick *joystick;



void catch_int(int sig_num) {
	printf( "Quiting...\n" );
    SDL_Quit();
	exit( 0 );
	signal( SIGINT, catch_int );
	}




void blowupOntoScreen( Uint32 *inImage, int inWidth, int inHeight,
                       int inBlowFactor, SDL_Surface *inScreen ) {
    
    int newWidth = inBlowFactor * inWidth;
    int newHeight =  inBlowFactor * inHeight;
    
    int yOffset = ( inScreen->h - newHeight ) / 2;
    int xOffset = ( inScreen->w - newWidth ) / 2;
    
    int endY = yOffset + newHeight;
    int endX = xOffset + newWidth;
    
    Uint32 *pixels = (Uint32 *)( inScreen->pixels );
    
    for( int y=yOffset; y<endY; y++ ) {
        int imageY = ( y - yOffset ) / inBlowFactor;
        
        for( int x=xOffset; x<endX; x++ ) {
            int imageX = ( x - xOffset ) / inBlowFactor;
            
            pixels[ y * inScreen->w + x ] =
                inImage[ imageY * inWidth + imageX ];
            }
        
        }
    
    }

// values in range 0..1
Image *tileImage;
int imageW, imageH;

// dimensions of one tile.  TileImage contains 13 tiles, stacked vertically,
// with blank lines between tiles
int tileW = 8;
int tileH = 8;


int numTileSets;

// how wide the swath of a world is that uses a given tile set
int tileSetWorldSpan = 200;
// overlap during tile set transition
int tileSetWorldOverlap = 50;



// optimization (found with profiler)
// values in range 0..255
double *tileRed;
double *tileGreen;
double *tileBlue;


Image *spriteImage;
int spriteW = 8;
int spriteH = 8;

double *spriteRed;
double *spriteGreen;
double *spriteBlue;

int currentSpriteIndex = 2;



double playerX, playerY;
int playerRadius = spriteW / 2;

int seed = time( NULL );



inline char isBlocked( int inX, int inY ) {
    // reduce to grid coordinates
    int gridX = inX / tileW;
    int gridY = inY / tileH;

    // wall along far left and top
    if( gridX <=0 || gridY <= 0 ) {
        return true;
        }

    // make a grid of empty spaces from which blocks can be 
    // removed below to make a maze
    if( gridX % 2 !=0 && gridY % 2 != 0 ) {
        return false;
        }
    

    // blocks get denser as y increases
    double threshold = 1 - gridY / 20.0;

    double randValue = noise4d( gridX, gridY, seed, 0 );
    return randValue > threshold;
    }


char isSpriteTransparent( int inSpriteIndex ) {
    // take transparent color from corner
    return 
        spriteRed[ inSpriteIndex ] == spriteRed[ 0 ]
        &&
        spriteGreen[ inSpriteIndex ] == spriteGreen[ 0 ]
        &&
        spriteBlue[ inSpriteIndex ] == spriteBlue[ 0 ];
    }



Uint32 sampleFromWorld( int inX, int inY, double inWeight = 1.0 ) {

    // consider sampling from sprite
    
    // player position centered on sprint left-to-right
    int spriteX = (int)( inX - playerX + spriteW / 2 );
    // player position at sprite's feet
    int spriteY = (int)( inY - playerY + spriteH - 1);

    
    if( spriteX >= 0 && spriteX < spriteW
        && 
        spriteY >= 0 && spriteY < spriteH ) {
        
        
        int spriteIndex = spriteY * spriteW + spriteX;
        
        // skip to appropriate sprite fram
        spriteIndex += currentSpriteIndex * spriteW * ( spriteH + 1 );
        
        if( !isSpriteTransparent( spriteIndex ) ) {
            
            unsigned char r = 
                (unsigned char)( 
                    inWeight * spriteRed[ spriteIndex ] );
    
            unsigned char g = 
                (unsigned char)( 
                    inWeight * spriteGreen[ spriteIndex ] );
    
            unsigned char b = 
                (unsigned char)( 
                    inWeight * spriteBlue[ spriteIndex ] );
            
            return r << 16 | g << 8 | b;
            }
        }
    

    int tileIndex;
    
    if( !isBlocked( inX, inY ) ) {
        // empty tile
        tileIndex = 0;
        }
    else {
        int neighborsBlockedBinary = 0;
        
        if( isBlocked( inX, inY - tileH ) ) {
            // top
            neighborsBlockedBinary = neighborsBlockedBinary | 1;
            }
        if( isBlocked( inX + tileW, inY ) ) {
            // right
            neighborsBlockedBinary = neighborsBlockedBinary | 1 << 1;
            }
        if( isBlocked( inX, inY + tileH ) ) {
            // bottom
            neighborsBlockedBinary = neighborsBlockedBinary | 1 << 2;
            }
        if( isBlocked( inX - tileW, inY ) ) {
            // left
            neighborsBlockedBinary = neighborsBlockedBinary | 1 << 3;
            }
        
        // skip empty tile, treat as tile index
        neighborsBlockedBinary += 1;
        
        tileIndex = neighborsBlockedBinary;
        }


    // pick a tile set
    int netWorldSpan = tileSetWorldSpan + tileSetWorldOverlap;

    int tileSet = inX / netWorldSpan;
    int overhang = inX % netWorldSpan;
    if( inX < 0 ) {
        // fix to a constant tile set below 0
        overhang = 0;
        tileSet = 0;
        }
    
    // is there blending with next tile set?
    int blendTileSet = tileSet + 1;
    double blendWeight = 0;
    
    if( overhang > tileSetWorldSpan ) {
        blendWeight = ( overhang - tileSetWorldSpan ) / 
            (double) tileSetWorldOverlap;
        }
    // else 100% blend of our first tile set


    tileSet = tileSet % numTileSets;
    blendTileSet = blendTileSet % numTileSets;
    
    // make sure not negative
    if( tileSet < 0 ) {
        tileSet += numTileSets;
        }
    if( blendTileSet < 0 ) {
        blendTileSet += numTileSets;
        }
    
    


    // sample from tile image
    int imageY = inY % tileH;
    int imageX = inX % tileW;
    

    if( imageX < 0 ) {
        imageX += tileW;
        }
    if( imageY < 0 ) {
        imageY += tileH;
        }
    
    // offset to top left corner of tile
    int tileImageOffset = tileIndex * ( tileH + 1 ) * imageW
        + tileSet * (tileW + 1);
    int blendTileImageOffset = tileIndex * ( tileH + 1 ) * imageW
        + blendTileSet * (tileW + 1);
    
    
    int imageIndex =  tileImageOffset + imageY * imageW + imageX;
    int blendImageIndex =  blendTileImageOffset + imageY * imageW + imageX;
    
    unsigned char r = 
        (unsigned char)( 
            inWeight * ( 
                (1-blendWeight) * tileRed[ imageIndex ] +
                blendWeight * tileRed[ blendImageIndex ] ) );
    
    unsigned char g = 
        (unsigned char)( 
            inWeight * ( 
                (1-blendWeight) * tileGreen[ imageIndex ] +
                blendWeight * tileGreen[ blendImageIndex ] ) );
    
    unsigned char b = 
        (unsigned char)( 
            inWeight * ( 
                (1-blendWeight) * tileBlue[ imageIndex ] +
                blendWeight * tileBlue[ blendImageIndex ] ) );
    
        
    return r << 16 | g << 8 | b;
    }



char getKeyDown( int inKeyCode ) {
    SDL_PumpEvents();
	Uint8 *keys = SDL_GetKeyState( NULL );
	return keys[ inKeyCode ] == SDL_PRESSED;
    }



char getHatDown( Uint8 inHatPosition ) {
    if( joystick == NULL ) {
        return false;
        }
    
    SDL_JoystickUpdate();

    Uint8 hatPosition = SDL_JoystickGetHat( joystick, 1 );
    
    if( hatPosition & inHatPosition ) {
        return true;
        }
    else {
        return false;
        }
    }



Image *readTGA( char *inFileName ) {
    File tileFile( NULL, inFileName );
    FileInputStream tileStream( &tileFile );
    
    TGAImageConverter converter;
    
    return converter.deformatImage( &tileStream );
    }




int mainFunction() {

    // let catch_int handle interrupt (^c)
    signal( SIGINT, catch_int );
    
    
    // read in map tile
    tileImage =  readTGA( "tileSet.tga" );
    
    imageW = tileImage->getWidth();
    imageH = tileImage->getHeight();
    
    numTileSets = (imageW + 1) / (tileW + 1);
    

    int imagePixelCount = imageW * imageH;
    
    tileRed =  new double[ imagePixelCount ];
    tileGreen =  new double[ imagePixelCount ];
    tileBlue =  new double[ imagePixelCount ];
    
    for( int i=0; i<imagePixelCount; i++ ) {
        tileRed[i] = 255 * tileImage->getChannel(0)[ i ];
        tileGreen[i] = 255 * tileImage->getChannel(1)[ i ];
        tileBlue[i] = 255 * tileImage->getChannel(2)[ i ];
        }
    

    // read in map tile
    spriteImage =  readTGA( "characterSprite.tga" );
    
    imagePixelCount = spriteImage->getWidth() * spriteImage->getHeight();
    
    spriteRed =  new double[ imagePixelCount ];
    spriteGreen =  new double[ imagePixelCount ];
    spriteBlue =  new double[ imagePixelCount ];
    
    for( int i=0; i<imagePixelCount; i++ ) {
        spriteRed[i] = 255 * spriteImage->getChannel(0)[ i ];
        spriteGreen[i] = 255 * spriteImage->getChannel(1)[ i ];
        spriteBlue[i] = 255 * spriteImage->getChannel(2)[ i ];
        }
        


    if( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_JOYSTICK |
                  SDL_INIT_NOPARACHUTE ) < 0 ) {
        printf( "Couldn't initialize SDL: %s\n", SDL_GetError() );
        return -1;
        }
    
    SDL_ShowCursor( SDL_DISABLE );
    
    Uint32 flags = SDL_HWSURFACE | SDL_DOUBLEBUF;
    if( fullScreen ) {
        flags = flags | SDL_FULLSCREEN;
        }
    

    SDL_Surface *screen = SDL_SetVideoMode( screenWidth, screenHeight, 
                                            32, 
                                            flags );
    
    if ( screen == NULL ) {
        printf( "Couldn't set %dx%dx32 video mode: %s\n", screenWidth, 
                screenHeight,
                SDL_GetError() );
        return-1;
        }

    // try to open joystick
    int numJoysticks = SDL_NumJoysticks();
    printf( "Found %d joysticks\n", numJoysticks );
    
    if( numJoysticks > 0 ) {
        // open first one by default
        joystick = SDL_JoystickOpen( 1 );
    
        if( joystick == NULL ) {
			printf( "Couldn't open joystick 1: %s\n", SDL_GetError() );
            }
        int numHats = SDL_JoystickNumHats( joystick );
        
        if( numHats <= 0 ) {
            printf( "No d-pad found on joystick\n" );
            SDL_JoystickClose( joystick );
            joystick = NULL;
            }
        }
    else {
        joystick = NULL;
        }
    

        
    Uint32 *pixels = (Uint32 *)( screen->pixels );
    
    Uint32 *gameImage = new Uint32[ width * height ];
    

    // first, fill the whole thing with black
    SDL_FillRect(screen, NULL, 0x00000000);

    // small area in center that we actually draw in, black around it
    int yOffset = ( screenHeight - height * blowUpFactor ) / 2;
    int xOffset = ( screenWidth - width * blowUpFactor ) / 2;
    

    double dX = 0;
    double dY = 0;

    int frameCount = 0;
    unsigned long startTime = time( NULL );
    
    char done = false;
    
    double maxWorldX = 0;
    double minWorldX = 0;


    // start player position
    playerX = tileW;
    //playerX = 0;
    dX = tileW;
    playerY = 0 + height/2;
    dY = 0;
    
    while( !done ) {

        char someBlocksDrawn = false;
        
        for( int y=0; y<height; y++ ) {
            for( int x=0; x<width; x++ ) {
                
                // offset into center
                int gameImageIndex = y * width + x;
                                

                
                //int worldX = (int)( pow( x / 2.0, 1.4 ) );
                
                // compression based on distance of pixel column from player
                double trueDistanceFromPlayer = dX + x - playerX;
                double maxDistance = width - 1;
                
                // cap it so that we don't force tan into infinity below
                double cappedDistanceFromPlayer = trueDistanceFromPlayer;
                if( cappedDistanceFromPlayer > maxDistance ) {
                    cappedDistanceFromPlayer = maxDistance;
                    }
                if( cappedDistanceFromPlayer < -maxDistance ) {
                    cappedDistanceFromPlayer = -maxDistance;
                    }
                
                // the world position we will sample from
                double worldX = x;

                // zone around player where no x compression happens
                int noCompressZone = 10;
                

                if( trueDistanceFromPlayer > noCompressZone ) {
                    
                    worldX =
                        x + 
                        //(width/8) *
                        // use true distance as a factor so that compression
                        // continues at constant rate after we pass capped 
                        // distance
                        // otherwise, compression stops after capped distance
                        // Still... constant rate looks weird, so avoid
                        // it by not letting it pass capped distance
                        trueDistanceFromPlayer / 2 *
                        ( pow( tan( ( ( cappedDistanceFromPlayer - 
                                        noCompressZone ) / 
                                      (double)( width - noCompressZone ) ) * 
                                    M_PI / 2 ), 2) );
                    /*
                        trueDistanceFromPlayer / 2 *
                        ( tan( ( cappedDistanceFromPlayer / 
                                 (double)( width - 0.5 ) ) * M_PI / 2 ) );
                    */
                        // simpler formula
                    // actually, this does not approach 0 as 
                    // cappedDistanceFromPlayer approaches 0, so use tan 
                    // instead
                    //    ( trueDistanceFromPlayer / 2 ) * 100
                    //  / pow( ( width - cappedDistanceFromPlayer ), 1.6 );
                    }
                else if( trueDistanceFromPlayer < - noCompressZone ) {
                    worldX =
                        x + 
                        //(width/8) *
                        trueDistanceFromPlayer / 2 *
                        ( pow( tan( ( ( - cappedDistanceFromPlayer - 
                                        noCompressZone ) / 
                                      (double)( width - noCompressZone ) ) * 
                                    M_PI / 2 ), 2) );
                        /*
                        trueDistanceFromPlayer / 2 * 
                        ( tan( ( - cappedDistanceFromPlayer / 
                                 (double)( width - 0.5 ) ) * M_PI / 2 ) );
                        */
                        //( trueDistanceFromPlayer / 2 ) * 50
                        /// ( width + cappedDistanceFromPlayer );
                    }
                else {
                    // inside no-compresison zone
                    worldX = x;
                    }
                
                
                //int worldX = x;
                
                worldX += dX;
                
                if( worldX > maxWorldX ) {
                    maxWorldX = worldX;
                    }
                if( worldX < minWorldX ) {
                    minWorldX = worldX;
                    }
                

                int worldY = (int)floor( y + dY );
                

                // linear interpolation of two samples for worldX
                int intWorldX = (int)floor( worldX );
                
                double bWeight = worldX - intWorldX;
                double aWeight = 1 - bWeight;
                
                
                Uint32 sampleA = 
                    sampleFromWorld( intWorldX, worldY, aWeight );
                Uint32 sampleB = 
                    sampleFromWorld( intWorldX + 1, worldY, bWeight );
                
                
                
                Uint32 combined = sampleB + sampleA;
                

                gameImage[ y * width + x ] = combined;
                
                }
            }

        
        // check if we need to lock the screen
        if( SDL_MUSTLOCK( screen ) ) {
            if( SDL_LockSurface( screen ) < 0 ) {
                printf( "Couldn't lock screen: %s\n", SDL_GetError() );
                }
            }
        
        
        blowupOntoScreen( gameImage, width, height, blowUpFactor, screen );
        
        
        // unlock the screen if necessary
        if ( SDL_MUSTLOCK(screen) ) {
            SDL_UnlockSurface(screen);
            }
	    
        if( ( screen->flags & SDL_DOUBLEBUF ) == SDL_DOUBLEBUF ) {
            // need to flip buffer
            SDL_Flip( screen );	
            printf( "double\n" );
            
            }
        else {
            // just update center
            //SDL_UpdateRect( screen, yOffset, xOffset, width, height );
            SDL_Flip( screen );	
            }

        int moveDelta = 1;
        if( getKeyDown( SDLK_LEFT ) || getHatDown( SDL_HAT_LEFT ) ) {
            if( currentSpriteIndex == 6 ) {
                currentSpriteIndex = 7;
                }
            else {
                currentSpriteIndex = 6;
                }
            
            if( !isBlocked( (int)( playerX - moveDelta ), (int)playerY ) ) {
                
                playerX -= moveDelta;
                
                if( playerX < 0 ) {
                    // undo
                    playerX += moveDelta;
                    }
                else {
                    // update screen position
                    dX -= moveDelta;
                    }
                }
            }
        else if( getKeyDown( SDLK_RIGHT ) || getHatDown( SDL_HAT_RIGHT )) {
            if( currentSpriteIndex == 2 ) {
                currentSpriteIndex = 3;
                }
            else {
                currentSpriteIndex = 2;
                }

            if( !isBlocked( (int)( playerX + moveDelta ), (int)playerY ) ) {
                
                dX += moveDelta;
                
                playerX += moveDelta;
                }
            }
        else if( getKeyDown( SDLK_UP ) || getHatDown( SDL_HAT_UP ) ) {
            if( currentSpriteIndex == 0 ) {
                currentSpriteIndex = 1;
                }
            else {
                currentSpriteIndex = 0;
                }
            
            if( !isBlocked( (int)playerX, (int)( playerY - moveDelta ) ) ) {
                
                playerY -= moveDelta;
                
                if( playerY < 0 ) {
                    // undo
                    playerY += moveDelta;
                    }
                else {
                    // update screen position
                    dY -= moveDelta;
                    }
                }
            }
        else if( getKeyDown( SDLK_DOWN )  || getHatDown( SDL_HAT_DOWN )) {
            if( currentSpriteIndex == 4 ) {
                currentSpriteIndex = 5;
                }
            else {
                currentSpriteIndex = 4;
                }
            
            if( !isBlocked( (int)playerX, (int)( playerY + moveDelta ) ) ) {
                
                dY += moveDelta;
                
                playerY += moveDelta;
                }
            }

        
        // check for events to quit
        SDL_Event event;
        while( SDL_PollEvent(&event) ) {
            switch( event.type ) {
                case SDL_KEYDOWN:
                    switch( event.key.keysym.sym ) {
                        case SDLK_q:
                            done = true;
                            break;
                        }
                    break;
                case SDL_QUIT:
                    done = true;
                    break;
                default:
                    break;
                }
            }

        //t +=0.25;
        frameCount ++;
        
        // player position on screen inches forward
        dX += timeDelta;
        //dX -= 1;
        // stop after player has gone off right end of screen
        if( playerX - dX > width ) {
            dX = playerX - width;
            }
        
        
        }
    
    unsigned long netTime = time( NULL ) - startTime;
    double frameRate = frameCount / (double)netTime;
    
    printf( "Max world x = %f\n", maxWorldX );
    printf( "Min world x = %f\n", minWorldX );
    
    printf( "Frame rate = %f fps (%d frames)\n", frameRate, frameCount );
    fflush( stdout );
    
    

    delete tileImage;
    delete [] tileRed;
    delete [] tileGreen;
    delete [] tileBlue;


    delete spriteImage;
    delete [] spriteRed;
    delete [] spriteGreen;
    delete [] spriteBlue;


    if( joystick != NULL ) {
        SDL_JoystickClose( joystick );
        }
    
    SDL_Quit();
    }
