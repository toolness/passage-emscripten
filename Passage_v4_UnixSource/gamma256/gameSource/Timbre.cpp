#include "Timbre.h"

#include "minorGems/util/stringUtils.h"

#include <math.h>
#include <stdio.h>


double twelthRootOfTwo = pow( 2, 1.0/12 );

// for major scale
// W, W, H, W, W, W, H
int halfstepMap[ 7 ] = { 0, 2, 4, 5, 7, 9, 11 };

// minor scale
// W,H,W,W,H,W,W
//int halfstepMap[ 7 ] = { 0, 2, 3, 5, 7, 8, 10 };



// gets frequency of note in our scale
double getFrequency( double inBaseFrequency, int inScaleNoteNumber ) {
    int octavesUp = inScaleNoteNumber / 7;
    
    int numHalfsteps = halfstepMap[ inScaleNoteNumber % 7 ] + octavesUp * 12;
    
    return inBaseFrequency * pow( twelthRootOfTwo, numHalfsteps );
    }




/*
  Was used during testing

#include "minorGems/sound/formats/aiff.h"

int outputFileNumber = 0;


// outputs a wave table as an AIFF
void outputWaveTable( Sint16 *inTable, int inLength, int inSampleRate ) {
    // generate the header
    int headerSize;
    unsigned char *aiffHeader =
        getAIFFHeader( 1,
                       16,
                       inSampleRate,
                       inLength, 
                       &headerSize );

    char *fileName = autoSprintf( "waveTable%d.aiff", outputFileNumber );
    outputFileNumber++;
    

    FILE *aiffFile = fopen( fileName, "wb" );

    delete [] fileName;
    

    //printf( "Header size = %d\n", headerSize );
    
    fwrite( aiffHeader, 1, headerSize, aiffFile );

    delete [] aiffHeader;
    
    for( int i=0; i<inLength; i++ ) {
        Sint16 val = inTable[i];
        
        unsigned char msb = val >> 8 & 0xFF;
        unsigned char lsb = val && 0xFF;

        fwrite( &msb, 1, 1, aiffFile );
        fwrite( &lsb, 1, 1, aiffFile );
        }
    fclose( aiffFile );
    
    }
*/




Timbre::Timbre( int inSampleRate,
                double inLoudness,
                double inBaseFrequency,
                int inNumWaveTableEntries, 
                double( *inWaveFunction )( double ) )
        : mNumWaveTableEntries( inNumWaveTableEntries ),
          mWaveTable( new Sint16*[ inNumWaveTableEntries ] ),
          mWaveTableLengths( new int[ inNumWaveTableEntries ] ) {

    // build wave table for each possible pitch in image
    
    for( int i=0; i<mNumWaveTableEntries; i++ ) {
        double freq = getFrequency( inBaseFrequency, i );
        double period = 1.0 / freq;
        
        // wave table contains more than one period to more 
        // accurately represent a signal with frequency freq
        int tableLength = (int)( 5 * period * inSampleRate );
        
        mWaveTableLengths[i] = tableLength;
        mWaveTable[i] = new Sint16[ tableLength ];

        // store double samples in temp table so we can compute
        // max value for normalization
        double *tempTable = new double[ tableLength ];
        double maxValue = 0;
        
        int s;
        
        for( s=0; s<tableLength; s++ ) {
            double t = (double)s / (double)inSampleRate;
            double waveValue = inWaveFunction( 2 * M_PI * t * freq );
        
            tempTable[s] = waveValue;
            
            // track max value
            if( waveValue > maxValue ) {
                maxValue = waveValue;
                }
            else if( -waveValue > maxValue ) {
                maxValue = -waveValue;
                }
            }
        
        // now normalize and convert to int
        for( s=0; s<tableLength; s++ ) {
            double waveValue = tempTable[s] * inLoudness / maxValue;
            
            // convert to int
            mWaveTable[i][s] = (Sint16)( 32767 * waveValue );
            }
        
        delete [] tempTable;
        
                
        mWaveTableLengths[i] = tableLength;
        

        // to examine waveforms for testing
        // outputWaveTable( mWaveTable[i], tableLength, inSampleRate );
        
        }
    }


        
Timbre::~Timbre() {
    delete [] mWaveTableLengths;
    
    for( int i=0; i<mNumWaveTableEntries; i++ ) {
        delete [] mWaveTable[i];
        }
    delete [] mWaveTable;
    }

