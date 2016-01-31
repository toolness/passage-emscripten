#include "Envelope.h"

#include <stdio.h>

// #include <assert.h>


Envelope::Envelope( double inAttackTime, double inDecayTime, 
                    double inSustainLevel, double inReleaseTime,
                    int inMaxNoteLengthInGridSteps,
                    int inGridStepDurationInSamples )
        : mNumComputedEnvelopes( inMaxNoteLengthInGridSteps ),
          mEvelopeLengths( new int[ inMaxNoteLengthInGridSteps ] ),
          mComputedEnvelopes( new double*[ inMaxNoteLengthInGridSteps ] ) {
    
    for( int i=0; i<mNumComputedEnvelopes; i++ ) {
        int length = (i+1) * inGridStepDurationInSamples;
        mEvelopeLengths[i] = length;
        
        mComputedEnvelopes[i] = new double[ length ];

        for( int s=0; s<length; s++ ) {
            
            double t = s / (double)( length - 1 );
            
            if( t < inAttackTime ) {
                // assert( inAttackTime > 0 );

                mComputedEnvelopes[i][s] = t / inAttackTime;
                }
            else if( t < inAttackTime + inDecayTime ) {
                // assert( inDecayTime > 0 );
                
                // decay down to sustain level
                mComputedEnvelopes[i][s] = 
                    ( 1.0 - inSustainLevel ) *
                    ( inAttackTime + inDecayTime - t ) / 
                    ( inDecayTime ) 
                    + inSustainLevel;
                }
            else if( 1.0 - t > inReleaseTime ) {
                mComputedEnvelopes[i][s] = inSustainLevel;
                }
            else {
                if( inReleaseTime > 0 ) {
                    
                    mComputedEnvelopes[i][s] = 
                        inSustainLevel -
                        inSustainLevel *
                        ( inReleaseTime - ( 1.0 - t ) ) / inReleaseTime;
                    }
                else {
                    // release time 0
                    // hold sustain until end
                    mComputedEnvelopes[i][s] = inSustainLevel;
                    }
                }
            
            }

        // test code to output evelopes for plotting in gnuplot
        if( false && i == 0 ) {
            FILE *file = fopen( "env0.txt", "w" );
            for( int s=0; s<length; s++ ) {
                fprintf( file, "%f %f\n", 
                         s / (double)( length - 1 ),
                         mComputedEnvelopes[i][s] );
                }
            fclose( file );
            }
        
        }
    
    }


              
Envelope::~Envelope() {
    for( int i=0; i<mNumComputedEnvelopes; i++ ) {
        delete [] mComputedEnvelopes[i];
        }
    delete [] mEvelopeLengths;
    delete [] mComputedEnvelopes;
    }



double *Envelope::getEnvelope( int inNoteLengthInGridSteps ) {
    return mComputedEnvelopes[ inNoteLengthInGridSteps - 1 ];
    }


