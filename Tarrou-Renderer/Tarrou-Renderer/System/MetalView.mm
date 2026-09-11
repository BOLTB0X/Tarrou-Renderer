//
//  MetalView.mm
//  Tarrou-Renderer
//
//  Created by B0X on 9/8/26.
//

#import "MetalView.h"
#import <AppKit/AppKit.h>
#import "imgui_impl_osx.h"

@implementation MetalView

- (BOOL)acceptsFirstResponder { return YES; }
- (BOOL)canBecomeKeyView { return YES; }

#pragma mark - Input Event Forwarding

- (void)keyDown:(NSEvent *)event {
    if (event.keyCode == 53) { // 53 = ESC 키
        [NSApp terminate:nil];
        return;
    }
    
    [super keyDown:event];
} // keyDown

@end
