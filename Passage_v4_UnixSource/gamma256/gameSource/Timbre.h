#include <stdint.h>
typedef int16_t Sint16;



class Timbre {
    public:
        
        /**
         * Constructs a timbre and fills its wavetables.
         *
         * @param inSampleRate number of samples per second.
         * @param inLoudness a scale factor in [0,1].
         * @param inBaseFrequency the lowest note in the wave table, in Hz.
         *   This is also the key for the major scale held in the wave table.
         * @param inNumWaveTableEntries the number of wavetable entries.
         * @param inWaveFunction a function mapping a double parameter t
         *   to a wave height in [-1,1].  Must have a period of 2pi.
         */
        Timbre( int inSampleRate,
                double inLoudness,
                double inBaseFrequency,
                int inNumWaveTableEntries, 
                double( *inWaveFunction )( double ) );
        
        ~Timbre();
        

        
        int mNumWaveTableEntries;
        // mWaveTable[x] corresponds to a wave with frequency of 
        // getFrequency(x)
        Sint16 **mWaveTable;
        int *mWaveTableLengths;
    };
