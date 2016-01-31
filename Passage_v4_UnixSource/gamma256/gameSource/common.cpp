#include "common.h"

#include "minorGems/graphics/converters/TGAImageConverter.h"

#include "minorGems/io/file/File.h"

#include "minorGems/io/file/FileInputStream.h"



Image *readTGA( const char *inFileName ) {
    return readTGA( "graphics", inFileName );
    }



Image *readTGA( const char *inFolderName, const char *inFileName ) {
    File tgaFile( new Path( inFolderName ), inFileName );
    FileInputStream tgaStream( &tgaFile );
    
    TGAImageConverter converter;
    
    return converter.deformatImage( &tgaStream );
    }
