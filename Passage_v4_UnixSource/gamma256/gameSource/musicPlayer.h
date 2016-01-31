

// starts playing music, reading notes from a TGA graphics file
// the file must be in the "music" directory
void startMusic( const char *inTGAFileName );



// set loudness in range [0.0,1.0]
void setMusicLoudness( double inLoudness );



// causes music to jump back to beginning
void restartMusic();



void stopMusic();

