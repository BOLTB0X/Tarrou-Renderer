//
//  Renderer.mm
//  Tarrou-Renderer
//
//  Created by B0X on 9/8/26.
//

#import "Application.h"
#import "MetalView.h"
#import "Base.h"
#import "Renderer.hpp"

#import "imgui.h"
#import "imgui_impl_metal.h"
#import "imgui_impl_osx.h"

@interface Application () {
    Base     *_base;     // 시간, FPS, GPU 칩셋 관리 객체
    Renderer *_renderer; // 순수 C++ 렌더러 객체
}

@property (nonatomic, strong) id<MTLDevice>       device;
@property (nonatomic, strong) id<MTLCommandQueue> commandQueue;
@property (nonatomic, weak)   MTKView            *metalView;

@end

@implementation Application

- (instancetype)initWithMetalKitView:(MTKView *)view {
    self = [super init];
    if (self) {
        _device = view.device;
        _commandQueue = [_device newCommandQueue];
        _metalView = view;

        // Base 및 C++ Renderer 초기화
        _base = [[Base alloc] initWithDevice:_device];
        CGSize drawableSize = view.drawableSize;
        _renderer = new Renderer();
        _renderer->Init((__bridge void*)_device, drawableSize.width, drawableSize.height);
        [self setupImGuiWithView:view];
    }
    return self;
} // initWithMetalKitView

- (void)setupImGuiWithView:(MTKView *)view {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    ImGui::StyleColorsDark();

    ImGui_ImplMetal_Init(self.device);
    ImGui_ImplOSX_Init(view);
} // setupImGuiWithView

- (void)dealloc {
    if (_renderer) {
        delete _renderer;
        _renderer = nullptr;
    }

    ImGui_ImplMetal_Shutdown();
    ImGui_ImplOSX_Shutdown();
    ImGui::DestroyContext();
} // dealloc

- (void)mtkView:(MTKView *)view drawableSizeWillChange:(CGSize)size {
    if (_renderer) {
        _renderer->OnResize(size.width, size.height);
    }
} // drawableSizeWillChange

- (void)drawInMTKView:(MTKView *)view {
    [_base tick];
    float dt = static_cast<float>(_base.deltaTime);

    MTLRenderPassDescriptor *descriptor = view.currentRenderPassDescriptor;
    if (!descriptor) { return; }

    ImGui_ImplMetal_NewFrame(descriptor);
    ImGui_ImplOSX_NewFrame(view);
    ImGui::NewFrame();

    Renderer::UpdateParam updateParam = {};
    updateParam.deltaTime = dt;
    
    ImGuiIO& io = ImGui::GetIO();
    
    if (!io.WantCaptureMouse) {
        if (ImGui::IsMouseDragging(ImGuiMouseButton_Right) || ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
            ImVec2 delta = ImGui::GetIO().MouseDelta;
            updateParam.rotationDeltaX = delta.x;
            updateParam.rotationDeltaY = delta.y;
        }
        updateParam.zoomDelta = io.MouseWheel;
    }
    
    if (!io.WantCaptureKeyboard) {
        if (ImGui::IsKeyDown(ImGuiKey_W)) updateParam.moveForward += dt;
        if (ImGui::IsKeyDown(ImGuiKey_S)) updateParam.moveForward -= dt;
        if (ImGui::IsKeyDown(ImGuiKey_D)) updateParam.moveRight += dt;
        if (ImGui::IsKeyDown(ImGuiKey_A)) updateParam.moveRight -= dt;
        if (ImGui::IsKeyDown(ImGuiKey_E)) updateParam.moveUp += dt;
        if (ImGui::IsKeyDown(ImGuiKey_Q)) updateParam.moveUp -= dt;
    }
    
    // 렌더러에 업데이트 파라미터 전달
    _renderer->Update(updateParam);

    const Renderer::ClearColor &cc = _renderer->GetClearColor();
    descriptor.colorAttachments[0].clearColor = MTLClearColorMake(cc.r, cc.g, cc.b, cc.a);

    id<MTLCommandBuffer> commandBuffer = [self.commandQueue commandBuffer];
    commandBuffer.label = @"TarrouRendererCommandBuffer";

    [self buildUI];
    ImGui::Render();

    id<MTLRenderCommandEncoder> encoder = [commandBuffer renderCommandEncoderWithDescriptor:descriptor];
    encoder.label = @"MainRenderEncoder";

    // 3D 그래픽스 렌더링
    _renderer->Render((__bridge void*)encoder);

    // ImGui UI 오버레이 렌더링
    ImGui_ImplMetal_RenderDrawData(ImGui::GetDrawData(), commandBuffer, encoder);

    [encoder endEncoding];

    id<CAMetalDrawable> drawable = view.currentDrawable;
    if (drawable) {
        [commandBuffer presentDrawable:drawable];
    }
    [commandBuffer commit];
}

- (void)buildUI {
    ImGui::Begin("Tarrou Renderer");
    
    // 하드웨어 및 파이프라인 지표
    ImGui::Text("Chipset: %s", [_base.chipsetName UTF8String]);
    ImGui::Text("Total Runtime: %.1f s", _base.totalTime);
    ImGui::Text("FPS: %.1f (%.2f ms)", _base.fps, _base.deltaTime * 1000.0);
    
    ImGui::Separator();
    
    // 메모리 점유율 지표
    ImGui::Text("RAM Usage: %.2f MB", _base.ramUsageMB);
    ImGui::Text("VRAM Usage: %.2f MB", _base.vramUsageMB);
    
    ImGui::Separator();
    
    // Clear Color 조작
    Renderer::ClearColor &cc = _renderer->GetClearColor();
    float color[4] = { cc.r, cc.g, cc.b, cc.a};
    if (ImGui::ColorEdit3("Clear Color", color)) {
        _renderer->SetClearColor(color[0], color[1], color[2], color[3]);
    }
    
    ImGui::Separator();
    if (ImGui::TreeNodeEx("Camera Control", ImGuiTreeNodeFlags_DefaultOpen)) {
        _renderer->OnGUI();
        ImGui::TreePop();
    }
    
    ImGui::End();
} // buildUI
@end
