#include "common.h"

#include "minorGems/graphics/converters/TGAImageConverter.h"

#include "minorGems/io/file/File.h"

#include "minorGems/io/file/FileInputStream.h"



Image *readTGA( char *inFileName ) {
	return readTGA( "graphics", inFileName );
    }



Image *readTGA( char *inFolderName, char *inFileName ) {
    // ignore passed-in path (use bundle root)
    File tgaFile( NULL, inFileName );
    FileInputStream tgaStream( &tgaFile );
    
    TGAImageConverter converter;
    
    return converter.deformatImage( &tgaStream );
    }
