

ROOT_PATH = ../../..

COMPILE = g++ -g -I${ROOT_PATH} -c
LINK = g++ -I${ROOT_PATH}



MINOR_GEMS_SOURCES = \
${ROOT_PATH}/minorGems/io/file/linux/PathLinux.cpp \
#${ROOT_PATH}/minorGems/graphics/linux/ScreenGraphicsLinux.cpp \
#${ROOT_PATH}/minorGems/system/win32/ThreadWin32.cpp \
#${ROOT_PATH}/minorGems/system/linux/ThreadLinux.cpp \

MINOR_GEMS_OBJECTS = ${MINOR_GEMS_SOURCES:.cpp=.o}

LIBRARIES = -lSDL


all: screenCompress tileSet.tga characterSprite.tga

screenCompress: screenCompress.o landscape.o ${MINOR_GEMS_OBJECTS}
	${LINK} -o screenCompress screenCompress.o landscape.o ${MINOR_GEMS_OBJECTS} ${LIBRARIES}


sdlTest: sdlTest.o
	${LINK} -o sdlTest sdlTest.o ${MINOR_GEMS_OBJECTS} ${LIBRARIES} -lSDLmain


screenCompress.o: screenCompress.cpp landscape.h

landscape.o: landscape.cpp landscape.h

tileSet.tga: tileSet.png
	convert tileSet.png tileSet.tga

characterSprite.tga: characterSprite.png
	convert characterSprite.png characterSprite.tga


# 
# Generic:
#
# Map all .cpp C++ and C files into .o object files
#
# $@   represents the name.o file
# $<   represents the name.cpp file
#
.cpp.o:
	${COMPILE} -o $@ $<
.c.o:
	${COMPILE} -o $@ $<
