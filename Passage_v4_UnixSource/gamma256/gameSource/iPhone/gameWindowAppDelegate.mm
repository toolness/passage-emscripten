//
//  testWindowAppAppDelegate.m
//  testWindowApp
//
//  Created by Jason Rohrer on 12/14/08.
//  Copyright __MyCompanyName__ 2008. All rights reserved.
//

#import "gameWindowAppDelegate.h"


@implementation gameWindowAppDelegate

@synthesize window;
@synthesize view;


- (void)applicationDidFinishLaunching:(UIApplication *)application {    
	
	printf( "App finished launching\n" );

    printf( "Calling start anim\n" );
	[view startAnimation];
    printf( "Done starting animation\n" );
    
    // Override point for customization after application launch
    [window makeKeyAndVisible];

    // Configure and start the accelerometer
    // off for now
    //[[UIAccelerometer sharedAccelerometer] setUpdateInterval:(1.0 / 15)];
    //[[UIAccelerometer sharedAccelerometer] setDelegate:self];
    


}


// UIAccelerometerDelegate method, called when the device accelerates.
- (void)accelerometer:(UIAccelerometer *)accelerometer didAccelerate:(UIAcceleration *)acceleration {
    //printf( "%d --- accel called w %f,%f\n", timeOfCall, acceleration.x, acceleration.y );
    // low pass filter
    float filterFactor = 0.5;
    accelerationBuffer[0] = acceleration.x * filterFactor + (1-filterFactor) * accelerationBuffer[0];
    accelerationBuffer[1] = acceleration.y * filterFactor + (1-filterFactor) * accelerationBuffer[1];
    accelerationBuffer[2] = acceleration.z * filterFactor + (1-filterFactor) * accelerationBuffer[2];
    
    setOrientation( asin( accelerationBuffer[0] ), asin( accelerationBuffer[1] ) );
    
}


- (void)dealloc {	
    [window release];
	[view stopAnimation];
	[view release];
	
    [super dealloc];
}

@end





#include "drawIntoScreen.h"

#import <QuartzCore/QuartzCore.h>
#import <OpenGLES/EAGLDrawable.h>

@implementation MyView

@synthesize animationTimer;
@synthesize context;

// You must implement this method
+ (Class)layerClass {
    return [CAEAGLLayer class];
}


//The GL view is stored in the nib file. When it's unarchived it's sent -initWithCoder:
- (id)initWithCoder:(NSCoder*)coder {
    
    if ((self = [super initWithCoder:coder])) {
        // Get the layer
        CAEAGLLayer *eaglLayer = (CAEAGLLayer *)self.layer;
        
        eaglLayer.opaque = YES;
        eaglLayer.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys:
                                        [NSNumber numberWithBool:NO], kEAGLDrawablePropertyRetainedBacking, kEAGLColorFormatRGBA8, kEAGLDrawablePropertyColorFormat, nil];
        
        context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES1];
        
        if (!context || ![EAGLContext setCurrentContext:context]) {
            [self release];
            return nil;
        }
        
    }
    return self;
}



NSTimeInterval countStartTime;
int appFrameCount = 0;

- (void)layoutSubviews {
    [EAGLContext setCurrentContext:context];
    [self destroyFramebuffer];
    [self createFramebuffer];
    
    NSDate *then = [NSDate date];
    
    countStartTime = [then timeIntervalSinceReferenceDate];
    
    [self drawFrame];
}


- (BOOL)createFramebuffer {
    
    glGenFramebuffersOES(1, &viewFramebuffer);
    glGenRenderbuffersOES(1, &viewRenderbuffer);
    
    glBindFramebufferOES(GL_FRAMEBUFFER_OES, viewFramebuffer);
    glBindRenderbufferOES(GL_RENDERBUFFER_OES, viewRenderbuffer);
    [context renderbufferStorage:GL_RENDERBUFFER_OES fromDrawable:(CAEAGLLayer*)self.layer];
    glFramebufferRenderbufferOES(GL_FRAMEBUFFER_OES, GL_COLOR_ATTACHMENT0_OES, GL_RENDERBUFFER_OES, viewRenderbuffer);
    
    glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES, GL_RENDERBUFFER_WIDTH_OES, &backingWidth);
    glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES, GL_RENDERBUFFER_HEIGHT_OES, &backingHeight);
    
    if(glCheckFramebufferStatusOES(GL_FRAMEBUFFER_OES) != GL_FRAMEBUFFER_COMPLETE_OES) {
        NSLog(@"failed to make complete framebuffer object %x", glCheckFramebufferStatusOES(GL_FRAMEBUFFER_OES));
        return NO;
    }
    
    glGenTextures( 1, &textureID );
    
    return YES;
}


- (void)destroyFramebuffer {
    
    glDeleteFramebuffersOES(1, &viewFramebuffer);
    viewFramebuffer = 0;
    glDeleteRenderbuffersOES(1, &viewRenderbuffer);
    viewRenderbuffer = 0;
    
    if(depthRenderbuffer) {
        glDeleteRenderbuffersOES(1, &depthRenderbuffer);
        depthRenderbuffer = 0;
    }
    
    glDeleteTextures( 1, &textureID );
}





- (void)setAnimationTimer:(NSTimer *)newTimer {
    [animationTimer invalidate];
    animationTimer = newTimer;
}

- (void)startAnimation {
	NSTimeInterval animationInterval = 1 / 15.0;
	
    self.animationTimer = [NSTimer scheduledTimerWithTimeInterval:animationInterval target:self selector:@selector(drawFrame) userInfo:nil repeats:YES];

    
    
    bitmapW = 128;
    bitmapH = 128;    
    
	int screenPixels = bitmapW * bitmapH;
    bitmapBytes = screenPixels* 4;

	screenBitmap = (Uint32 *) malloc( bitmapBytes );
    /*
     // for old DrawImage version
	provider = CGDataProviderCreateWithData( NULL, screenBitmap, screenPixels * 4, NULL );
	colorSpaceRef = CGColorSpaceCreateDeviceRGB();
	imageRef = CGImageCreate(
								  bitmapW,
								  bitmapH,
								  8,
								  32,
								  4 * bitmapW,
								  colorSpaceRef,
								  kCGImageAlphaNoneSkipLast,
								  provider,
								  NULL,
								  NO,
								  kCGRenderingIntentDefault );
     */
	initScreenDrawer( screenBitmap, bitmapW, bitmapH );
}


- (void)stopAnimation {
	printf( "Stop anim called\n" );
    self.animationTimer = nil;
	
    free( screenBitmap );
    
	freeScreenDrawer();
}










- (void)drawFrame {
    // old DrawImage version
	//[self setNeedsDisplay];
    
    
    
    
    const GLfloat squareVertices[] = {
        -1.6f, -1.6f,
        1.6f,  -1.6f,
        -1.6f,  1.6f,
        1.6f,   1.6f,
    };
    /*
    const GLubyte squareColors[] = {
        255, 255,   0, 255,
        0,   255, 255, 255,
        255,     0,   0,   0,
        255,   0, 255, 255,
    };
     */
    const GLfloat squareTextureCoords[] = {
        0, 0,
        1,  0,
        0,  1,
        1,   1
    };
    
    [EAGLContext setCurrentContext:context];
    
    glBindFramebufferOES(GL_FRAMEBUFFER_OES, viewFramebuffer);
    glViewport(0, 0, backingWidth, backingHeight);
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrthof(-1.0f, 1.0f, -1.5f, 1.5f, -1.0f, 1.0f);
    
    
    glMatrixMode(GL_MODELVIEW);
    
    //glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    //glClear(GL_COLOR_BUFFER_BIT);
    
    glVertexPointer(2, GL_FLOAT, 0, squareVertices);
    glEnableClientState(GL_VERTEX_ARRAY);
    
    //glColorPointer(4, GL_UNSIGNED_BYTE, 0, squareColors);
    //glEnableClientState(GL_COLOR_ARRAY);
    
    glTexCoordPointer(2, GL_FLOAT, 0, squareTextureCoords);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    

    // set new texture data
    drawIntoScreen( screenBitmap, bitmapW, bitmapH );
    
    int error;
    
	GLenum texFormat = GL_RGBA;
	glBindTexture( GL_TEXTURE_2D, textureID );
    
    error = glGetError();
	if( error != GL_NO_ERROR ) {		// error
		printf( "Error binding to texture id %d, error = %d\n",
               (int)textureID,
               error );
    }
    
    
	glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
    
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
    
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    
    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
    
	glTexImage2D( GL_TEXTURE_2D, 0,
                 texFormat, bitmapW,
                 bitmapH, 0,
                 texFormat, GL_UNSIGNED_BYTE, screenBitmap );
    
	error = glGetError();
	if( error != GL_NO_ERROR ) {		// error
		printf( "Error setting texture data for id %d, error = %d\n",
               (int)textureID, error );
        printf( "Perhaps texture image width or height is not a power of 2\n"
               "Width = %lu, Height = %lu\n",
               bitmapW, bitmapH );
    }
    
    
    
    
    glEnable( GL_TEXTURE_2D );
	glBindTexture( GL_TEXTURE_2D, textureID );
    
    
    
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    
    glBindRenderbufferOES(GL_RENDERBUFFER_OES, viewRenderbuffer);
    [context presentRenderbuffer:GL_RENDERBUFFER_OES];
    
    
    appFrameCount++;
    
    // disable FPS tracking
    if( false && appFrameCount > 100 ) {
        NSDate *now = [NSDate date];
        
        NSTimeInterval newStartTime = [now timeIntervalSinceReferenceDate]; 
        
        NSTimeInterval elapsedTime = newStartTime - countStartTime;
        
        printf( "FPS: %f\n", appFrameCount / elapsedTime );
        
        countStartTime = newStartTime;
        appFrameCount = 0;
    }

}




/*

// old DrawImage version 
 
 #include <time.h>
 
 unsigned int appFrameCountStartTime = time( NULL );
 
 int appFrameCount = 0;
 
- (void)drawRect:(CGRect)rect {
	
	//printf( "Draw Rect called!\n" );

	drawIntoScreen( screenBitmap, bitmapW, bitmapH );
	
	CGContextRef context = UIGraphicsGetCurrentContext();
	
    //CGContextRotateCTM ( context, M_PI / 2 );
    //CGContextTranslateCTM ( context, 0, -bitmapH );

    CGRect imageRect = CGRectMake ( 0, 0, bitmapW, bitmapH );
    
	CGContextDrawImage(context, imageRect, imageRef );
	
	appFrameCount++;
	
	if( appFrameCount > 100 ) {
		unsigned int newTime = time( NULL );
		unsigned int timeDelta = newTime - appFrameCountStartTime;
		
		printf( "FPS = %f\n", (double)appFrameCount / (double)timeDelta );
		appFrameCount = 0;
		appFrameCountStartTime = newTime;
	}
     
}
*/

// Handles the start of a touch
- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
	CGRect				bounds = [self bounds];
    UITouch*	touch = [[event touchesForView:self] anyObject];
	
    //Convert touch point from UIView referential to OpenGL one (upside-down flip)
	CGPoint	location = [touch locationInView:self];
	location.y = bounds.size.height - location.y;
    
    touchStartPoint( location.x, location.y );
}



// Handles touch motion
- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event
{
	CGRect				bounds = [self bounds];
    UITouch*	touch = [[event touchesForView:self] anyObject];
	
    //Convert touch point from UIView referential to OpenGL one (upside-down flip)
	CGPoint	location = [touch locationInView:self];
	location.y = bounds.size.height - location.y;
    
    touchMovePoint( location.x, location.y );
}


// Handles touch end
- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
	CGRect				bounds = [self bounds];
    UITouch*	touch = [[event touchesForView:self] anyObject];
	
    //Convert touch point from UIView referential to OpenGL one (upside-down flip)
	CGPoint	location = [touch locationInView:self];
	location.y = bounds.size.height - location.y;
    
    touchEndPoint( location.x, location.y );
}


@end
