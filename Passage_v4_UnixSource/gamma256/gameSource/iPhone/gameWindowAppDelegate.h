//
//  testWindowAppAppDelegate.h
//  testWindowApp
//
//  Created by Jason Rohrer on 12/14/08.
//  Copyright __MyCompanyName__ 2008. All rights reserved.
//

#include "drawIntoScreen.h"

#import <UIKit/UIKit.h>
#import <OpenGLES/EAGL.h>
#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>

@interface MyView : UIView {
@private
	NSTimer *animationTimer;
	
    /* The pixel dimensions of the backbuffer */
    GLint backingWidth;
    GLint backingHeight;
    
    EAGLContext *context;
    
    /* OpenGL names for the renderbuffer and framebuffers used to render to this view */
    GLuint viewRenderbuffer, viewFramebuffer;
    
    /* OpenGL name for the depth buffer that is attached to viewFramebuffer, if it exists (0 if it does not exist) */
    GLuint depthRenderbuffer;
    
    GLuint textureID;
    
    
	
	Uint32 *screenBitmap;
    int bitmapW;
    int bitmapH;
    int bitmapBytes;
    
    // for old DrawImage version
    // CGDataProviderRef provider;
	// CGColorSpaceRef colorSpaceRef;
	// CGImageRef imageRef;
}

@property NSTimer *animationTimer;
@property (nonatomic, retain) EAGLContext *context;

- (void)startAnimation;
- (void)stopAnimation;
- (void)drawFrame;
- (BOOL) createFramebuffer;
- (void) destroyFramebuffer;

@end


@interface gameWindowAppDelegate : NSObject <UIAccelerometerDelegate> {
    UIWindow *window;
	MyView *view;
    // used for low-pass filtering
    UIAccelerationValue accelerationBuffer[3];
}

@property (nonatomic, retain) IBOutlet UIWindow *window;
@property (nonatomic, retain) IBOutlet MyView *view;



@end



