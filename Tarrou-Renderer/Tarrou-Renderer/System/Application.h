//
//  Renderer.h
//  Tarrou-Renderer
//
//  Created by B0X on 9/8/26.
//

#import <MetalKit/MetalKit.h>

@interface Application : NSObject <MTKViewDelegate>

- (instancetype)initWithMetalKitView:(MTKView *)view;

@end
