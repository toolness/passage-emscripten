#ifndef COMMON_INCLUDED
#define COMMON_INCLUDED



#include "minorGems/graphics/Image.h"


// reads a TGA file from the default ("graphics") folder
Image *readTGA( const char *inFileName );


Image *readTGA( const char *inFolderName, const char *inFileName );



#endif
