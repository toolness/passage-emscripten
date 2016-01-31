/*
 * Modification History
 *
 * 2006-September-26   Jason Rohrer
 * Switched to cosine interpolation.
 * Added optimizations discovered with profiler.  Reduced running time by 18%.
 */



#include "landscape.h"

#include <math.h>
#include <stdlib.h>

#include "minorGems/util/SimpleVector.h"



double landscape( double inX, double inY, double inT,
                  double inBaseFrequency, double inRoughness,
                  int inDetail ) {
    
    if( inDetail < 0 ) {
        return 0.0;
        }
    else {
        // frequency of octave
        double frequency = inBaseFrequency * pow( 2, inDetail );
        double amplitude = pow( inRoughness, inDetail );
        return amplitude * noise4d( inX * frequency, 
                                    inY * frequency,
                                    // index different planes of noise
                                    inDetail,  
                                    inT )
            + landscape( inX, inY, inT, inBaseFrequency, inRoughness,
                         inDetail - 1 );
        }
    }



double variableRoughnessLandscape( double inX, double inY, double inT,
                                   double inBaseFrequency,
                                   double inRoughnessChangeFrequency,
                                   double inMinRoughness,
                                   double inMaxRoughness,
                                   int inDetail ) {

    double roughnessFreqX = inX * inRoughnessChangeFrequency;
    double roughnessFreqY = inY * inRoughnessChangeFrequency;

    // use low-frequency noise 4d to select landscape roughness
    // between 0 and 1
    double roughness =
        ( noise4d( 6, roughnessFreqX, roughnessFreqY, inT ) + 1 ) / 2;

    // move roughness into specified range
    roughness =
        roughness * ( inMaxRoughness - inMinRoughness ) +
        inMinRoughness;

    return landscape( inX, inY, inT, inBaseFrequency, roughness, inDetail );
    }



int getRandomPoints( double inStartX, double inEndX,
                     double inStartY, double inEndY,
                     double inT,
                     double inSampleStepSize,
                     double inDensity,
                     double **outXCoordinates,
                     double **outYCoordinates ) {

    SimpleVector<double> *xCoordinates = new SimpleVector<double>();
    SimpleVector<double> *yCoordinates = new SimpleVector<double>();

    // discretize startX and start Y so that sample grid for differently-placed
    // windows always meshes
    // use ceil to ensure that starting points are always inside the
    // inStart/inEnd bounds
    double discretizedStartX =
        inSampleStepSize * ceil( inStartX / inSampleStepSize );
    double discretizedStartY =
        inSampleStepSize * ceil( inStartY / inSampleStepSize );

    // put a point wherever we have a zero-crossing
    double lastSample = 1;
    
    for( double x=discretizedStartX; x<=inEndX; x+=inSampleStepSize ) {
        for( double y=discretizedStartY; y<=inEndY; y+=inSampleStepSize ) {
            double landscapeSample =
                variableRoughnessLandscape(
                    30 * x + 1000, 30 * y + 1000, inT + 1000,
                    0.01, 0.001, 0.25, 0.65, 0 );

            // shift landscape up to reduce chance of zero-crossing
            landscapeSample = (1-inDensity) * 0.5 + 0.5 + landscapeSample ;
            
            if( landscapeSample < 0 &&
                lastSample >= 0 ||
                landscapeSample >= 0 &&
                landscapeSample < 0 ) {

                // sign change

                // hit
                xCoordinates->push_back( x );
                yCoordinates->push_back( y );
                }

            lastSample = landscapeSample;
            }
        }

    *outXCoordinates = xCoordinates->getElementArray();
    *outYCoordinates = yCoordinates->getElementArray();

    int numPoints = xCoordinates->size();
    
    delete xCoordinates;
    delete yCoordinates;

    return numPoints;
    }



/**
 * Computes a 32-bit random number.
 * Use linear congruential method.
 *
 * @param inSeed the seed to use.
 */
// this is the readable version of the funcion
// it has been turned into a set of macros below
inline unsigned int random32_readable( unsigned int inSeed ) {
    // this is the true hot-spot of the entire landscape function
    // thus, optimization is warranted.
    
    // multiplier = 3141592621
    // use hex to avoid warnings
    //unsigned int multiplier = 0xBB40E62D;
    //unsigned int increment = 1;

    // better:
    // unsigned int multiplier = 196314165
    // unsigned int increment  = 907633515
    
    // this will automatically be mod-ed by 2^32 because of the limit
    // of the unsigned int type
    // return multiplier * inSeed + increment;
    //return 0xBB40E62D * inSeed + 1;
    //return 196314165 * inSeed + 907633515;
    
    //int n = ( inSeed << 13 ) ^ inSeed;
    //return n * (n * n * 15731 + 789221) + 1376312589;

    //const unsigned int Num1 = (inSeed * 0xFEA09B9DU) + 1;
	//const unsigned int Num2 = ((inSeed * 0xB89C8895U) + 1) >> 16;
	//return Num1 ^ Num2;

    /*
    unsigned int rseed=(inSeed*15064013)^(inSeed*99991+604322121)^(inSeed*45120321)^(inSeed*5034121+13);

    const unsigned int Num1 = (inSeed * 0xFEA09B9DU) + 1;

    const unsigned int Num2 = ((inSeed * 0xB89C8895U) + 1) >> 16;

    rseed *= Num1 ^ Num2;

    return rseed;
    */

    const unsigned int Num1 = (inSeed * 0xFEA09B9DU) + 1;
	const unsigned int Num2 = ((inSeed^Num1) * 0x9C129511U) + 1;
	const unsigned int Num3 = (inSeed * 0x2512CFB8U) + 1;
	const unsigned int Num4 = ((inSeed^Num3) * 0xB89C8895U) + 1;
	const unsigned int Num5 = (inSeed * 0x6BF962C1U) + 1;
	const unsigned int Num6 = ((inSeed^Num5) * 0x4BF962C1U) + 1;

	return Num2 ^ (Num4 >> 11) ^ (Num6 >> 22);
    }


// faster as a set of macros
#define Num1( inSeed ) \
    ( ( inSeed * 0xFEA09B9DU ) + 1 )

#define Num2( inSeed ) \
    ( ( ( inSeed ^ Num1( inSeed ) ) * 0x9C129511U ) + 1 )

#define Num3( inSeed ) \
    ( ( inSeed * 0x2512CFB8U ) + 1 )

#define Num4( inSeed ) \
    ( ( ( inSeed ^ Num3( inSeed ) ) * 0xB89C8895U ) + 1 )

#define Num5( inSeed ) \
    ( ( inSeed * 0x6BF962C1U ) + 1 )

#define Num6( inSeed ) \
    ( ( ( inSeed ^ Num5( inSeed ) ) * 0x4BF962C1U ) + 1 )

#define random32( inSeed ) \
    ( Num2( inSeed ) ^ (Num4( inSeed ) >> 11) ^ (Num6( inSeed ) >> 22) )







#define invMaxIntAsDouble 2.32830643708e-10
// 1/(x/2) = 2*(1/x)
//double invHalfMaxIntAsDouble = 2 * invMaxIntAsDouble;
//  2.32830643708e-10
//+ 2.32830643708e-10
//-------------------
//  4.65661287416e-10
#define invHalfMaxIntAsDouble 4.65661287416e-10



#define mixFour( x, y, z, t ) ( x ^ (y * 57) ^ (z * 131) ^ (t * 2383) )



/**
 * Maps 4d integer coordinates into a [-1..1] noise space.
 *
 * @param x, y, z, t  the 4d coordinates.
 *
 * @return a random value in the range [-1..1]
 */
// keep readable version around for reference
// it has been replaced by a macro below
inline double noise4dInt32_readable( unsigned int x,
                                     unsigned int y,
                                     unsigned int z,
                                     unsigned int t ) {

    
    //double maxIntAsDouble = 4294967295.0;
    
    // modular addition automatic
    // multiply x, y, z, and t by distinct primes to
    // avoid correllations.
    // using xor ( ^ ) here seems to avoid correllations that show
    // up when using addition.
        
    // mix x, y, z, and t
    unsigned int randomSeed =
        x ^
        y * 57 ^
        z * 131 ^
        t * 2383;

    // a random value between 0 and max unsigned int
    unsigned int randomValue = random32( randomSeed );

    // a random value between 0 and 2
    double zeroTwoValue = randomValue * invHalfMaxIntAsDouble;

    // a random value between -1 and 1
    return zeroTwoValue - 1;    
    }


// noise4dInt32 function call itself was the slowest spot in code
// (found with profiler)
// turn into a set of macros

// matches original parameter format 
#define noise4dInt32( x, y, z, t ) \
    random32( mixFour( x, y, z, t ) ) \
    * invHalfMaxIntAsDouble - 1

// problem:  now that random32 is a macro, we are passing the unevaluated
// expression, ( x ^ (y * 57) ^ (z * 131) ^ (t * 2383) ), down into it.
// it is being evaluated 6 times within the depths of the random32 macro

// thus, we need to provide a new format where the caller can precompute
// the mix for us.  This is even faster.
#define noise1dInt32( precomputedMix ) \
    random32( precomputedMix ) \
    * invHalfMaxIntAsDouble - 1




/*
 * The following functions (blendNoiseNd) do 4d linear interpolation
 * one dimension at a time.
 *
 * The end result is 8 calls to blendNoise1d (and 16 calls to noise4dInt32).
 *
 * This method was inspired by the functional implementations---I am
 * decomposing a complicated problem into sub-problems that are easier
 * to solve.
 */


// faster than f * b + (1-f) * a
// one less multiply
#define linearInterpolation( t, a, b ) ( a + t * ( b - a ) )


/**
 * Blends 4d discrete (integer-parameter) noise function along one dimension
 * with 3 fixed integer parameters.
 */
inline double blendNoise1d( double x,
                     unsigned int y,
                     unsigned int z,
                     unsigned int t ) {

    double floorX = floor( x );
    unsigned int floorIntX = (unsigned int)floorX;
    
    if( floorX == x ) {
        unsigned int precomputedMix = mixFour( floorIntX, y, z, t );
        
        return noise1dInt32( precomputedMix );
        }
    else {
        unsigned int ceilIntX = floorIntX + 1;

        // cosine interpolation
        // from http://freespace.virgin.net/hugo.elias/models/m_perlin.htm
        double ft = ( x - floorX ) * M_PI;
        double f = ( 1 - cos( ft ) ) * .5;
        
        
        // need to pre-store intermediate values because noise4dInt32 is a
        // macro
        // thus, we end up calling the noise1dInt32 function instead
        
        unsigned int precomputedMix = mixFour( floorIntX, y, z, t );
        double valueAtFloor = noise1dInt32( precomputedMix );

        precomputedMix = mixFour( ceilIntX, y, z, t );
        double valueAtCeiling = noise1dInt32( precomputedMix );
        
        return linearInterpolation( f, valueAtFloor, valueAtCeiling );
        }
    }



/**
 * Blends 4d discrete (integer-parameter) noise function along 2 dimensions
 * with 2 fixed integer parameters.
 */
double blendNoise2d( double x,
                     double y,
                     unsigned int z,
                     unsigned int t ) {

    double floorY = floor( y );
    unsigned int floorIntY = (unsigned int)floorY;
    
    if( floorY == y ) {
        return blendNoise1d( x, floorIntY, z, t );
        }
    else {
        unsigned int ceilIntY = floorIntY + 1;

        // cosine interpolation
        // from http://freespace.virgin.net/hugo.elias/models/m_perlin.htm
        double ft = ( y - floorY ) * M_PI;
        double f = ( 1 - cos( ft ) ) * .5;
        
        return ( f ) * blendNoise1d( x, ceilIntY, z, t ) +
               ( 1 - f ) * blendNoise1d( x, floorIntY, z, t );
        }
    }



/**
 * Blends 4d discrete (integer-parameter) noise function along 3 dimensions
 * with 1 fixed integer parameters.
 */
double blendNoise3d( double x,
                     double y,
                     double z,
                     unsigned int t ) {

    double floorZ = floor( z );
    unsigned int floorIntZ = (unsigned int)floorZ;
    
    if( floorZ == z ) {
        return blendNoise2d( x, y, floorIntZ, t );
        }
    else {
        unsigned int ceilIntZ = floorIntZ + 1;
        
        // cosine interpolation
        // from http://freespace.virgin.net/hugo.elias/models/m_perlin.htm
        double ft = ( z - floorZ ) * M_PI;
        double f = ( 1 - cos( ft ) ) * .5;
        
        return ( f ) * blendNoise2d( x, y, ceilIntZ, t ) +
               ( 1 - f ) * blendNoise2d( x, y, floorIntZ, t );
        }
    }



/**
 * Blends 4d discrete (integer-parameter) noise function along 4 dimensions.
 */
double noise4d( double x,
                double y,
                double z,
                double t ) {

    double floorT = floor( t );
    unsigned int floorIntT = (unsigned int)floorT;
    
    if( floorT == t ) {
        return blendNoise3d( x, y, z, floorIntT );
        }
    else {
        unsigned int ceilIntT = floorIntT + 1;

        // cosine interpolation
        // from http://freespace.virgin.net/hugo.elias/models/m_perlin.htm
        double ft = ( t - floorT ) * M_PI;
        double f = ( 1 - cos( ft ) ) * .5;
        
        return ( f ) * blendNoise3d( x, y, z, ceilIntT ) +
               ( 1 - f ) * blendNoise3d( x, y, z, floorIntT );
        }
    }



/**
 * Blends 4d discrete (integer-parameter) noise function along 3 dimensions
 * to get a 3D noise function.
 */
double noise3d( double x,
                double y,
                double z ) {
    return blendNoise3d( x, y, z, 0 );
    }





