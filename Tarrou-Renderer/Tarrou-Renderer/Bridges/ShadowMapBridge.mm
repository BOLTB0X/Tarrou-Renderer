//
//  ShadowMapBridge.mm
//  Tarrou-Renderer
//
//  Created by B0X on 9/20/26.
//

#import <Metal/Metal.h>
#import <CoreFoundation/CoreFoundation.h>

extern "C" void* ShadowMapBridge_CreateShadowDepthTexture(void* devicePtr, int width, int height) {
    id<MTLDevice> device = (__bridge id<MTLDevice>)devicePtr;
    
    MTLTextureDescriptor* desc = [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:MTLPixelFormatDepth32Float
                                     width:width
                                    height:height
                                 mipmapped:NO];
    
    // RenderTarge으로도 쓰고, 나중에 Shader에서 읽을 수 있게 권한 설정
    desc.usage = MTLTextureUsageRenderTarget | MTLTextureUsageShaderRead;
    desc.storageMode = MTLStorageModePrivate; // GPU 전용 메모리
    
    id<MTLTexture> depthTexture = [device newTextureWithDescriptor:desc];
    return (void*)CFBridgingRetain(depthTexture);
} // RendererBridge_CreateShadowDepthTexture

extern "C" void* ShadowMapBridge_CreateShadowPassDescriptor(void* depthTexturePtr) {
    id<MTLTexture> depthTexture = (__bridge id<MTLTexture>)depthTexturePtr;
    if (!depthTexture) return nullptr;
    
    MTLRenderPassDescriptor* passDescriptor = [MTLRenderPassDescriptor renderPassDescriptor];
    
    // 그림자 패스는 색상을 칠할 필요가 없으므로 Depth만 설정
    passDescriptor.depthAttachment.texture = depthTexture;
    passDescriptor.depthAttachment.loadAction = MTLLoadActionClear;    // 매 프레임 시작 시 초기화
    passDescriptor.depthAttachment.storeAction = MTLStoreActionStore;  // 렌더링 결과를 텍스처에 저장
    passDescriptor.depthAttachment.clearDepth = 0.0f;
    
    return (void*)CFBridgingRetain(passDescriptor);
} // RendererBridge_CreateShadowPassDescriptor

extern "C" void ShadowMapBridge_ReleaseMetalObject(void* metalObject) {
    if (metalObject) {
        CFBridgingRelease(metalObject);
    }
} // ShadowMapBridge_ReleaseMetalObject

extern "C" bool ShadowMapBridge_InitShadowPipeline(void*       devicePtr,
                                                   const char* shaderSourceRaw,
                                                   const char* objectFunctionName,
                                                   const char* meshFunctionName,
                                                   size_t      payloadSize,
                                                   void**      outPipelineState) {
    @autoreleasepool {
        id<MTLDevice> device = (__bridge id<MTLDevice>)devicePtr;
        NSString* shaderSource = [NSString stringWithUTF8String:shaderSourceRaw];
        NSError* error = nil;
        
        id<MTLLibrary> customLibrary = [device newLibraryWithSource:shaderSource options:nil error:&error];
        if (!customLibrary) {
            NSLog(@"[BuddhaBridge] Shadow Shader Compile Error: %@", error);
            return false;
        }
        
        id<MTLFunction> objectFunc = [customLibrary newFunctionWithName:[NSString stringWithUTF8String:objectFunctionName]];
        id<MTLFunction> meshFunc = [customLibrary newFunctionWithName:[NSString stringWithUTF8String:meshFunctionName]];
        
        MTLMeshRenderPipelineDescriptor* pipelineDesc = [[MTLMeshRenderPipelineDescriptor alloc] init];
        pipelineDesc.objectFunction = objectFunc;
        pipelineDesc.meshFunction = meshFunc;
        pipelineDesc.payloadMemoryLength = payloadSize;
        
        pipelineDesc.colorAttachments[0].pixelFormat = MTLPixelFormatInvalid;
        pipelineDesc.depthAttachmentPixelFormat = MTLPixelFormatDepth32Float;
        
        id<MTLRenderPipelineState> pipelineState = [device newRenderPipelineStateWithMeshDescriptor:pipelineDesc options:0 reflection:nil error:&error];
        
        if (!pipelineState) {
            NSLog(@"[BuddhaBridge] Shadow Pipeline creation error: %@", error);
            return false;
        }
        
        *outPipelineState = (void*)CFBridgingRetain(pipelineState);
        return true;
    }
} // ShadowMapBridge_InitShadowPipeline
