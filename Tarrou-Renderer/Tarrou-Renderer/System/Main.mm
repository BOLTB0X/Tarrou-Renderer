//
//  Main.mm
//  Tarrou-Renderer
//
//  Created by B0X on 9/8/26.
//

#import <Cocoa/Cocoa.h>
#import "AppDelegate.h"

int main(int argc, const char * argv[]) {
    @autoreleasepool {
        NSApplication *app = [NSApplication sharedApplication];
        AppDelegate *delegate = [[AppDelegate alloc] init];
        app.delegate = delegate;

        [app setActivationPolicy:NSApplicationActivationPolicyRegular];

        [app run];
    }
    return 0;
} // main
