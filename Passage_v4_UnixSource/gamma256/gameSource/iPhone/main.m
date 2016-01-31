//
//  main.m
//  testWindowApp
//
//  Created by Jason Rohrer on 12/14/08.
//  Copyright __MyCompanyName__ 2008. All rights reserved.
//

#import <UIKit/UIKit.h>



int main(int argc, char *argv[]) {
    
    //printf( "Arg 0 = %s\n", argv[0] );
    
    // arg 0 is the path to the app executable
    char *appDirectoryPath = strdup( argv[ 0 ] );
    
    //printf( "Mac:  app path %s\n", appDirectoryPath );
    
    char *bundleName = "Passage.app";
    
    char *appNamePointer = strstr( appDirectoryPath, bundleName );
    
    if( appNamePointer != NULL ) {
        // terminate full app path to get bundle directory
        appNamePointer[ strlen( bundleName ) ] = '\0';
        
        printf( "Mac: changing working dir to %s\n", appDirectoryPath );
        chdir( appDirectoryPath );
    }
    
    free( appDirectoryPath );
    
    
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
    int retVal = UIApplicationMain(argc, argv, nil, nil);
    [pool release];
    return retVal;
}
