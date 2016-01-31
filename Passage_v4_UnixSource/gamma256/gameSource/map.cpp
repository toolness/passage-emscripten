#include "map.h"

#include "landscape.h"


#include "minorGems/util/SimpleVector.h"

#include <time.h>



int seed = time( NULL );
//int seed = 10;

extern int tileH;
extern int tileW;



char isBlockedGrid( int inGridX, int inGridY, char inDoNotRecurse );
char isBlockedGridHash( int inGridX, int inGridY, char inDoNotRecurse );




char allNeighborsBlocked( int inGridX, int inGridY ) {
    if( ! isBlockedGridHash( inGridX, inGridY - 1, true ) ) {
        // top 
        return false;
        }
    if( ! isBlockedGridHash( inGridX + 1, inGridY, true ) ) {
        // right
        return false;
        }
    if( ! isBlockedGridHash( inGridX, inGridY + 1, true ) ) {
            // bottom
        return false;
        }
    if( ! isBlockedGridHash( inGridX - 1, inGridY, true ) ) {
        // left
        return false;
        }
    
    // all neighbors blocked
    return true;
    }



#include "HashTable.h"

HashTable<char> blockedHashTable( 3000 );


char isBlockedGridHash( int inGridX, int inGridY, char inDoNotRecurse ) {

    char found;
    
    char blocked = blockedHashTable.lookup( inGridX, inGridY, &found );
    
    if( found ) {
        return blocked;
        }
    
    // else not found
    
    // call real function to get result
    blocked = isBlockedGrid( inGridX, inGridY, inDoNotRecurse );
    
    // only insert result if we called the full recursive version of 
    // inBlockGrid.  Otherwise, we aren't getting the real result back
    // so we shouldn't be tracking it in our table
    if( ! inDoNotRecurse ) {
        blockedHashTable.insert( inGridX, inGridY, blocked );
        }
    
    return blocked;
    }



char isBlocked( int inX, int inY ) {
    // reduce to grid coordinates
    int gridX = inX / tileW;
    int gridY = inY / tileH;
    
    // try hash first
    return isBlockedGridHash( gridX, gridY, false );
    }



char isBlockedGrid( int inGridX, int inGridY, char inDoNotRecurse ) {
    
    // wall along far left and top
    if( inGridX <=0 || inGridY <= 0 ) {
        return true;
        }

    // empty passage at top
    if( inGridY <= 1 ) {
        return false;
        }


    // make a grid of empty spaces from which blocks can be 
    // removed below to make a maze
    if( inGridX % 2 != 0 && inGridY % 2 != 0 ) {

        // however, if all neighbors are blocked, this should
        // be blocked too
        if( !inDoNotRecurse ) {
            return allNeighborsBlocked( inGridX, inGridY );
            }
        else {
            // non-recursive mode
            return false;
            }
        }
    

    // blocks get denser as y increases
    double threshold = 1 - inGridY / 20.0;

    double randValue = noise3d( inGridX, inGridY, seed );
    char returnValue = randValue > threshold;

    if( returnValue ) {
        return true;
        }
    else {
        // not blocked

        // should be blocked if all neighbors are blocked
        if( !inDoNotRecurse ) {
            return allNeighborsBlocked( inGridX, inGridY );
            }
        else {
            // non-recursive mode
            return false;
            }
        }
    
    }


struct pairStruct {
        int x;
        int y;
    };
typedef struct pairStruct pair;



SimpleVector<pair> openedChests;



char isChest( int inX, int inY ) {
    
    // reduce to grid coordinates
    int gridX = inX / tileW;
    int gridY = inY / tileH;

    // chests get denser as y increases
    // no chests where gridY < 5 
    // even less dense than blocks
    double threshold = 1 - ( gridY - 5 ) / 200.0;

    // use different seed than for blocks
    double randValue = noise3d( 73642 * gridX, 283277 * gridY, seed * 987423 );
    char returnValue = randValue > threshold;

    if( returnValue ) {
        // make sure it's not opened already
        for( int i=0; i<openedChests.size(); i++ ) {
            pair *p = openedChests.getElement( i );
            if( p->x == gridX && p->y == gridY ) {
                return CHEST_OPEN;
                }
            }
        return CHEST_CLOSED;
        }
    else {
        return CHEST_NONE;
        }
    }



unsigned char getChestCode( int inX, int inY ) {
    // reduce to grid coordinates
    int gridX = inX / tileW;
    int gridY = inY / tileH;
    
    // use different seed
    double randValue = noise3d( 37462 * gridX, 1747 * gridY, seed * 3748147 );
    

    /*
      Note:
      Original versions of Passage (up to v3 for Mac/PC/Unix) contained a bug.
      
      This was the original code:
      
        // use it to fill first 6 binary digits
        return (unsigned char)( randValue * 15 ) & 0x3F;
    
      Note two things:
      1. That randValue is *signed* (and can be negative), so casting
         it to an uchar is a weird thing to do.

      2. That we're only multiplying randValue by 15, which only fills
         the first 4 binary digits (there used to be only 4 gems).
         Thus, if randValue is actually *positive* between 0 and 1, only
         the first 4 gems have a chance of being activated.

      What this code should have done was convert randValue to the range
      [0..1] first and then multiply it by 63 (to convert it to a 6-bit
      random binary string).

      However, this code *seems* to work, anyway, since 2's compliment was 
      being invoked to handle casting of negative randValues, so all 6 gems
      were sometimes being activated.

      However, the 6-bit gem strings were always of the form 00XXXX or 11XXXX
      (where XXXX is a random 4-bit sting) due to the 2's compliment and
      the fact that the negative numbers being complimented were never less
      than -15.

      So, 2 of the gems were tied together in their on/off status, essentially
      turning 6 gems into 5.

      The problem appeared on platforms (like iPhone) that don't preserve
      2's compliment bits when doing signed to unsigned casting.  On these
      platforms, all negative numbers were being casted to 0 when turned to
      uchars.  Thus, half the treasure chests, on average, had no gems at all.

      Of course, a proper fix, as described above, would go beyond just making 
      it work on these platforms, but would also change the behavior of the game
      (those last two gems would no longer be tied together in their on-off
      status).
     
      After much deliberation, I came to the following solution:

      The following code emulates 2's compliment casting, whether the platform
      does it or not, making behavior on all platforms identical.

      (And preserving the original bug!!)
    */

    unsigned char castedChar;
    if( randValue < 0 ) {
        castedChar = (unsigned char)( 256 + (char)( randValue * 15 ) );
        }
    else {
        castedChar = (unsigned char)( randValue * 15 );
        }

    return castedChar & 0x3F;
    }



void getChestCenter( int inX, int inY, int *outCenterX, int *outCenterY ) {
    int gridX = inX / tileW;
    int gridY = inY / tileH;
    
    *outCenterX = gridX * tileW + tileW / 2;
    *outCenterY = gridY * tileH + tileH / 2;
    }




void openChest( int inX, int inY ) {
    pair p;

    // reduce to grid coordinates
    p.x = inX / tileW;
    p.y = inY / tileH;
    
    openedChests.push_back( p );
    }



void resetMap() {
    // new seed
    seed = time( NULL );
    //seed = 10;

    openedChests.deleteAll();
    
    blockedHashTable.clear();
    }

