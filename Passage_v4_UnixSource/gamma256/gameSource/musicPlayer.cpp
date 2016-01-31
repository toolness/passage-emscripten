
#include "common.h"
#include "Timbre.h"
#include "Envelope.h"

#include "minorGems/util/SimpleVector.h"



#include <SDL/SDL.h>
#include <SDL/SDL_audio.h>

#include <math.h>
#include <stdlib.h>

int sampleRate = 22050;
//int sampleRate = 11025;




Image *musicImage = NULL;
int w, h;

// total number of samples played so far
int streamSamples = 0;

// offset into grid at start
// for testing
int gridStartOffset = 0;



// overal loudness of music
double musicLoudness = 1.0;






// one grid step in seconds
double gridStepDuration = 0.25;
int gridStepDurationInSamples = (int)( gridStepDuration * sampleRate );

double entireGridDuraton;


// c
double keyFrequency = 261.63;


int numTimbres = 4;

Timbre *musicTimbres[ 4 ];

int numEnvelopes = 4;

Envelope *musicEnvelopes[ 4 ];



class Note {
    public:
        // index into musicTimbres array
        int mTimbreNumber;
        
        // index into musicEnvelopes array
        int mEnvelopeNumber;
                
        int mScaleNoteNumber;
        
        // additional loudness adjustment
        // places note in stereo space
        double mLoudnessLeft;
        double mLoudnessRight;
        

        // start time, in seconds from start of note grid
        double mStartTime;

        // duration in seconds
        double mDuration;

        // used when note is currently playing to track progress in note
        // negative if we should wait before starting to play the note
        int mCurrentSampleNumber;

        // duration in samples
        int mNumSamples;
        
                
    };


// isomorphic to our music image, except only has an entry for each note
// start (all others, including grid spots that contain note continuations,
//  are NULL)
// indexed as noteGrid[y][x]
Note ***noteGrid;


SimpleVector<Note*> currentlyPlayingNotes;



// need to synch these with audio thread

void setMusicLoudness( double inLoudness ) {
    SDL_LockAudio();
    
    musicLoudness = inLoudness;
    
    SDL_UnlockAudio();
    }



void restartMusic() {
    SDL_LockAudio();

    // return to beginning (and forget samples we've played so far)
    streamSamples = 0;
    
    // drop all currently-playing notes
    currentlyPlayingNotes.deleteAll();
        
    SDL_UnlockAudio();
    }




// called by SDL to get more samples
void audioCallback( void *inUserData, Uint8 *inStream, int inLengthToFill ) {
    
    // 2 bytes for each channel of stereo sample
    int numSamples = inLengthToFill / 4;
    

    Sint16 *samplesL = new Sint16[ numSamples ];
    Sint16 *samplesR = new Sint16[ numSamples ];
    
    // first, zero-out the buffer to prepare it for our sum of note samples
    // each sample is 2 bytes
    memset( samplesL, 0, 2 * numSamples );
    memset( samplesR, 0, 2 * numSamples );
    

    int i;


    // hop through all grid steps that *start* in this stream buffer
    // add notes that start during this stream buffer

    // how far into stream buffer before we hit our first grid step? 
    int startOfFirstGridStep = streamSamples % gridStepDurationInSamples;
    
    if( startOfFirstGridStep != 0 ) {
        startOfFirstGridStep = 
            gridStepDurationInSamples - startOfFirstGridStep;
        }
    

    // hop from start of grid step to start of next grid step
    // ignore samples in between, since notes don't start there,
    // and all we're doing right now is finding notes that start
    for( i=startOfFirstGridStep; 
         i<numSamples; 
         i += gridStepDurationInSamples ) {
        
        // start of new grid position

        // check for new notes that are starting
        
        // map into our music image:
        int x = ( streamSamples + i ) / gridStepDurationInSamples;
            
        // wrap in image
        x = x % w;

        for( int y=0; y<h; y++ ) {
            
            Note *note = noteGrid[y][x];
                
            if( note != NULL ) {
                // new note
                currentlyPlayingNotes.push_back( note );
                // start it
                
                // set a delay for its start based on our position
                // in this callback buffer
                note->mCurrentSampleNumber = -i;
                }
            }
        }
    
    streamSamples += numSamples;
    

    // loop over all current notes and add their samples to buffer
        
    for( int n=0; n<currentlyPlayingNotes.size(); n++ ) {
            
        Note *note = *( currentlyPlayingNotes.getElement( n ) );
             
        int waveTableNumber = note->mScaleNoteNumber;
        Timbre *timbre = musicTimbres[ note->mTimbreNumber ];
        int tableLength = timbre->mWaveTableLengths[ waveTableNumber ];
            
        Sint16 *waveTable = timbre->mWaveTable[ waveTableNumber ];
        
        Envelope *env = musicEnvelopes[ note->mEnvelopeNumber ];
        double *envLevels = 
            env->getEnvelope( 
                // index envelope by number of grid steps in note
                note->mNumSamples / gridStepDurationInSamples );
        
        
        double noteLoudnessL = note->mLoudnessLeft;
        double noteLoudnessR = note->mLoudnessRight;
        
        // do this outside inner loop
        noteLoudnessL *= musicLoudness;
        noteLoudnessR *= musicLoudness;
        

        int noteStartInBuffer = 0;
        int noteEndInBuffer = numSamples;
        
        if( note->mCurrentSampleNumber < 0 ) {
            // delay before note starts in this sample buffer
            noteStartInBuffer = - note->mCurrentSampleNumber;
            
            // we've taken account of the delay
            note->mCurrentSampleNumber = 0;
            }

        char endNote = false;
        
        int numSamplesLeftInNote = 
            note->mNumSamples - note->mCurrentSampleNumber;
        
        if( noteStartInBuffer + numSamplesLeftInNote < noteEndInBuffer ) {
            // note ends before end of buffer
            noteEndInBuffer = noteStartInBuffer + numSamplesLeftInNote;
            endNote = true;
            }
        

        int waveTablePos = note->mCurrentSampleNumber % tableLength;
        
        int currentSampleNumber = note->mCurrentSampleNumber;
        
        for( i=noteStartInBuffer; i != noteEndInBuffer; i++ ) {
            double envelope = envLevels[ currentSampleNumber ];
            
            double monoSample = envelope * 
                waveTable[ waveTablePos ];
            

            samplesL[i] += (Sint16)( noteLoudnessL * monoSample );
            samplesR[i] += (Sint16)( noteLoudnessR * monoSample );
            
            currentSampleNumber ++;
            
            waveTablePos ++;
            
            // avoid using mod operator (%) in inner loop
            // found with profiler
            if( waveTablePos == tableLength ) {
                // back to start of table
                waveTablePos = 0;
                }
            
            }
        
        note->mCurrentSampleNumber += ( noteEndInBuffer - noteStartInBuffer );
        
        if( endNote ) {
            // note ended in this buffer
            currentlyPlayingNotes.deleteElement( n );
            n--;
            }
        
        }


    // now copy samples into Uint8 buffer
    int streamPosition = 0;
    for( i=0; i != numSamples; i++ ) {
        Sint16 intSampleL = samplesL[i];
        Sint16 intSampleR = samplesR[i];
        
        inStream[ streamPosition ] = (Uint8)( intSampleL & 0xFF );
        inStream[ streamPosition + 1 ] = (Uint8)( ( intSampleL >> 8 ) & 0xFF );
        
        inStream[ streamPosition + 2 ] = (Uint8)( intSampleR & 0xFF );
        inStream[ streamPosition + 3 ] = (Uint8)( ( intSampleR >> 8 ) & 0xFF );
        
        streamPosition += 4;
        }

    delete [] samplesL;
    delete [] samplesR;
    
    }



// limit on n, based on Nyquist, when summing sine components
//int nLimit = (int)( sampleRate * M_PI );
// actually, this is way too many:  it takes forever to compute
// use a lower limit instead
// This produces fine results (almost perfect square wave)
int nLimit = 40;



// square wave with period of 2pi
double squareWave( double inT ) {
    double sum = 0;
    
    for( int n=1; n<nLimit; n+=2 ) {
        sum += 1.0/n * sin( n * inT );
        }
    return sum;
    }



// sawtoot wave with period of 2pi
double sawWave( double inT ) {
    double sum = 0;
    
    for( int n=1; n<nLimit; n++ ) {
        sum += 1.0/n * sin( n * inT );
        }
    return sum;
    }


// white noise, ignores inT
double whiteNoise( double inT ) {
    return 2.0 * ( rand() / (double)RAND_MAX ) - 1.0;
    }


// white noise where each sample is averaged with last sample
// effectively a low-pass filter
double lastSample = 0;

double smoothedWhiteNoise( double inT ) {
    // give double-weight to last sample to make it even smoother
    lastSample = ( 2 * lastSample + whiteNoise( inT ) ) / 3;
    
    return lastSample;
    }




void loadMusicImage( const char *inTGAFileName ) {
        
    musicImage = readTGA( "music", inTGAFileName );

    w = musicImage->getWidth();
    h = musicImage->getHeight();

    // notes are in red and green channel
    double *redChannel = musicImage->getChannel( 0 );
    double *greenChannel = musicImage->getChannel( 1 );

    
    entireGridDuraton = gridStepDuration * w;
    

    // jump ahead in stream, if needed
    streamSamples += gridStartOffset * gridStepDurationInSamples;
    
    
    // blank line of pixels between timbres
    int heightPerTimbre = (h+1) / numTimbres - 1;


    // find the maximum number of simultaneous notes in the song
    // take loudness into account
    double  maxNoteLoudnessInAColumn = 0;
    
    int x, y;
    for( x=0; x<w; x++ ) {
        double noteLoudnessInColumnL = 0;
        double noteLoudnessInColumnR = 0;
        
        for( y=0; y<h; y++ ) {
            
            int imageIndex = y * w + x;
            
            // the note number in our scale
            // scale starts over for each timbre, with blank line
            // in between timbres
            int noteNumber = (h - y - 1) % (heightPerTimbre + 1);

            if( // not blank line between timbres
                noteNumber < heightPerTimbre &&  
                // tone present in image
                ( redChannel[ imageIndex ] > 0 || 
                  greenChannel[ imageIndex ] > 0 ) ) {
                
                noteLoudnessInColumnL += greenChannel[ imageIndex ];
                noteLoudnessInColumnR += redChannel[ imageIndex ];
                
                }
            }
        // pick loudest channel for this column and compare it to
        // loudest column/channel seen so far
        if( maxNoteLoudnessInAColumn < noteLoudnessInColumnL ) {
            maxNoteLoudnessInAColumn = noteLoudnessInColumnL;
            }
        if( maxNoteLoudnessInAColumn < noteLoudnessInColumnR ) {
            maxNoteLoudnessInAColumn = noteLoudnessInColumnR;
            }
        
        }
    
   
    // divide loudness amoung timbres to avoid clipping
    double loudnessPerTimbre = 1.0 / maxNoteLoudnessInAColumn;
    
    // further adjust loudness per channel here as we construct
    // each timbre.
    // This is easier than tweaking loundness of a given part by hand
    // using a painting program

    musicTimbres[0] = new Timbre( sampleRate, 0.6 * loudnessPerTimbre,
                                  keyFrequency,
                                  heightPerTimbre, sawWave );
    musicTimbres[1] = new Timbre( sampleRate, loudnessPerTimbre,
                                  keyFrequency,
                                  heightPerTimbre, sin );
    musicTimbres[2] = new Timbre( sampleRate, 0.4 * loudnessPerTimbre,
                                  keyFrequency / 4,
                                  heightPerTimbre, squareWave );
    musicTimbres[3] = new Timbre( sampleRate, 0.75 * loudnessPerTimbre,
                                  keyFrequency / 4,
                                  heightPerTimbre, smoothedWhiteNoise );


    // next, compute the longest note in the song
    int maxNoteLength = 0;
    
    for( y=0; y<h; y++ ) {
        int currentNoteLength = 0;
        
        for( x=0; x<w; x++ ) {
            int imageIndex = y * w + x;
            
            // the note number in our scale
            // scale starts over for each timbre, with blank line
            // in between timbres
            int noteNumber = (h - y - 1) % (heightPerTimbre + 1);

            if( // not blank line between timbres
                noteNumber < heightPerTimbre &&  
                // tone present in image
                ( redChannel[ imageIndex ] > 0 ||
                  greenChannel[ imageIndex ] > 0 ) ) {
                
                currentNoteLength ++;
                }
            else {
                currentNoteLength = 0;
                }
            if( currentNoteLength > maxNoteLength ) {
                maxNoteLength = currentNoteLength;
                }
            }
        }
    
    printf( "Max note length in song = %d\n", maxNoteLength );
    

    
    musicEnvelopes[0] = new Envelope( 0.05, 0.7, 0.25, 0.1,
                                      maxNoteLength,
                                      gridStepDurationInSamples );
    musicEnvelopes[1] = new Envelope( 0.1, 0.9, 0.0, 0.0,
                                      maxNoteLength,
                                      gridStepDurationInSamples );
    musicEnvelopes[2] = new Envelope( 0.25, 0.0, 1.0, 0.1,
                                      maxNoteLength,
                                      gridStepDurationInSamples );
    musicEnvelopes[3] = new Envelope( 0.0, 0.2, 0.0, 0.0,
                                      maxNoteLength,
                                      gridStepDurationInSamples );
    


    
    noteGrid = new Note**[ h ];
    
    for( int y=0; y<h; y++ ) {
        noteGrid[y] = new Note*[ w ];
        
        // each row is one pitch for a given instrument
        // thus, two consecutive pixels should be the same note
        // handle this by tracking whether a note is playing or not
        char notePlaying = false;
        Note *noteStart = NULL;
        for( int x=0; x<w; x++ ) {
            int imageIndex = y * w + x;
            
            // default to NULL
            noteGrid[y][x] = NULL;

            // the note number in our scale
            // scale starts over for each timbre, with blank line
            // in between timbres
            int noteNumber = (h - y - 1) % (heightPerTimbre + 1);

            

            if( // not blank line between timbres
                noteNumber < heightPerTimbre &&  
                // tone present in image
                ( redChannel[ imageIndex ] > 0 || 
                  greenChannel[ imageIndex ] > 0 ) ) {
               

                if( notePlaying ) {
                    // part of note that's already playing
                    
                    // one more grid step
                    noteStart->mDuration += gridStepDuration;
                    noteStart->mNumSamples += gridStepDurationInSamples;
                    
                    }
                else {
                    // start a new note
                    noteGrid[y][x] = new Note();
                    
                    noteGrid[y][x]->mScaleNoteNumber = noteNumber;
                    
                    noteGrid[y][x]->mTimbreNumber = 
                        y / ( heightPerTimbre + 1 );
            
                    // same as timbre number
                    noteGrid[y][x]->mEnvelopeNumber = 
                        noteGrid[y][x]->mTimbreNumber;
                    
                    // left loudness from green brightness
                    noteGrid[y][x]->mLoudnessLeft = greenChannel[ imageIndex ];
                    
                    // right loudness from red brightness
                    noteGrid[y][x]->mLoudnessRight = redChannel[ imageIndex ];
                    
                    noteGrid[y][x]->mStartTime = gridStepDuration * x;
                    
                    // one grid step so far
                    noteGrid[y][x]->mDuration = gridStepDuration;
                    noteGrid[y][x]->mNumSamples = gridStepDurationInSamples;
                    
                    // track if it needs to be continued
                    notePlaying = true;
                    noteStart = noteGrid[y][x];
                    }
                }
            else {
                // no tone

                if( notePlaying ) {
                    // stop it
                    notePlaying = false;
                    noteStart = NULL;
                    }
                }
            }
        }
    
        
    }



void startMusic( const char *inTGAFileName ) {
    
    loadMusicImage( inTGAFileName );
    
    SDL_AudioSpec audioFormat;

    /* Set 16-bit stereo audio at 22Khz */
    audioFormat.freq = sampleRate;
    audioFormat.format = AUDIO_S16;
    audioFormat.channels = 2;
    audioFormat.samples = 512;        /* A good value for games */
    audioFormat.callback = audioCallback;
    audioFormat.userdata = NULL;

    /* Open the audio device and start playing sound! */
    if( SDL_OpenAudio( &audioFormat, NULL ) < 0 ) {
        printf( "Unable to open audio: %s\n", SDL_GetError() );
        }

    // set pause to 0 to start audio
    SDL_PauseAudio(0);
    
    
    }



void stopMusic() {
    SDL_CloseAudio();

    if( musicImage != NULL ) {
        delete musicImage;
        musicImage = NULL;
        }

    for( int y=0; y<h; y++ ) {

        for( int x=0; x<w; x++ ) {
            
            if( noteGrid[y][x] != NULL ) {
                delete noteGrid[y][x];
                }
            }
        delete [] noteGrid[y];
        }
    
    delete [] noteGrid;

    
    int i;
    
    for( i=0; i<numTimbres; i++ ) {
        delete musicTimbres[i];
        }
    for( i=0; i<numEnvelopes; i++ ) {
        delete musicEnvelopes[i];
        }
    
    }

    
