//
//  AppDelegate.mm
//  Tarrou-Renderer
//
//  Created by B0X on 9/8/26.
//

#import "AppDelegate.h"
#import "MetalView.h"
#import "Application.h"

@interface AppDelegate ()

@property (nonatomic, strong) NSWindow    *window;
@property (nonatomic, strong) MetalView   *metalView;
@property (nonatomic, strong) Application *app;

@end

@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)notification {
    const NSRect frame = NSMakeRect(0, 0, 1280, 720);

    self.window = [[NSWindow alloc] initWithContentRect:frame
                                               styleMask:(NSWindowStyleMaskTitled |
                                                           NSWindowStyleMaskClosable |
                                                           NSWindowStyleMaskResizable |
                                                           NSWindowStyleMaskMiniaturizable)
                                                 backing:NSBackingStoreBuffered
                                                   defer:NO];
    self.window.title = @"Tarrou Renderer";
    [self.window center];

    [self.window setAcceptsMouseMovedEvents:YES];

    id<MTLDevice> device = MTLCreateSystemDefaultDevice();
    if (!device) {
        NSLog(@"이 기기에서 Metal을 지원하지 않습니다.");
        [NSApp terminate:nil];
        return;
    }

    // 순정 MTKView 대신, 이벤트를 ImGui로 포워딩하는 MetalView 사용
    self.metalView = [[MetalView alloc] initWithFrame:frame device:device];
    self.metalView.colorPixelFormat = MTLPixelFormatBGRA8Unorm;
    self.metalView.depthStencilPixelFormat = MTLPixelFormatDepth32Float;
    self.metalView.enableSetNeedsDisplay = NO;
    self.metalView.paused = NO;
    self.metalView.clearDepth = 0.0;

    self.app = [[Application alloc] initWithMetalKitView:self.metalView];
    self.metalView.delegate = self.app;

    self.window.contentView = self.metalView;
    [self.window makeFirstResponder:self.metalView];
    [self.window makeKeyAndOrderFront:nil];

    [NSApp activateIgnoringOtherApps:YES];
} // applicationDidFinishLaunching

- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)sender {
    return YES;
} // applicationShouldTerminateAfterLastWindowClosed

@end
