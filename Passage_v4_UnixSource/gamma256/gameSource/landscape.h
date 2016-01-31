


#ifndef LANDSCAPE_INCLUDED
#define LANDSCAPE_INCLUDED



/**
 * Gets height samples from an "infinite" fractal landscape.
 * The landscape can change over time by varying t.
 *
 * @param x the x coordinate of the sample.
 * @param y the y coordinate of the sample.
 * @param t the time of the sample.
 * @param baseFrequency the frequency to use for the lowest detail component.
 * @param inRoughness the roughness of the landscape (how much high frequencies
 *   are factored in).  Should be in the range [0..1] with 0 making a very
 *   smooth landscape and 1 making a very rough landscape.
 * @param detail the detail level.  Larger numbers result in more
 *   detail.  Defaults to 10.
 *
 * @return the height of the landscape at the sample point/time.
 */
double landscape( double inX, double inY, double inT,
                  double inBaseFrequency, double inRoughness,
                  int inDetail = 10 );



/**
 * Samples height of a landscape that varies in roughness over the xy plane.
 *
 * @params same as for landscape, except:
 * @param inRoughnessChangeFrequency the rate at which roughness changes
 *  over space.  Should, in general, be less than inBaseFrequency.
 * @param inMinRoughness the minimum roughness value, in the range [0..1].
 * @param inMaxRoughness the maximum roughness value, in the range [0..1].
 *
 * @return same as for landscape.
 */
double variableRoughnessLandscape( double inX, double inY, double inT,
                                   double inBaseFrequency,
                                   double inRoughnessChangeFrequency,
                                   double inMinRoughness,
                                   double inMaxRoughness,
                                   int inDetail );



/**
 * Computes linearly-blended random values in the range [-1..1] from a
 * 4d parameterized noise space.
 *
 * @param x, y, z, t  the 4d floating-point coordinates.
 *
 * @return a blended random value in the range [-1..1].
 */
double noise4d( double x,
                double y,
                double z,
                double t );



/**
 * Computes linearly-blended random values in the range [-1..1] from a
 * 4d parameterized noise space (keeping one dimension constant).
 *
 * Should be faster than noise4D.
 *
 * @param x, y, z  the 3d floating-point coordinates.
 *
 * @return a blended random value in the range [-1..1].
 */
double noise3d( double x,
                double y,
                double z );



/**
 * Gets a set of randomly-chosen (though stable) points in a given
 * region of the landscape.
 *
 * @param inStartX, inEndX the x region.
 * @param inStartY, inEndY the y region.
 * @param inT the time.
 * @param inSampleStepSize the step size in the sample grid.
 *   Higher values are faster but result in sparser distributions of points.
 * @param inDensity the density of points, in the range [0,1].
 * @param outXCoordinates pointer to where array of x coordinates should
 *   be returned.  Array must be destroyed by caller.
 * @param outYCoordinates pointer to where array of x coordinates should
 *   be returned.  Array must be destroyed by caller.
 *
 * @return the number of points (the length of outXCoordinates).
 */
int getRandomPoints( double inStartX, double inEndX,
                     double inStartY, double inEndY,
                     double inT,
                     double inSampleStepSize,
                     double inDensity,
                     double **outXCoordinates,
                     double **outYCoordinates );
                     


#endif




