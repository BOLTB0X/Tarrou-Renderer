//
//  Base.mm
//  Tarrou-Renderer
//
//  Created by B0X on 9/8/26.
//

#import "Base.h"
#import <QuartzCore/QuartzCore.h>
#import <mach/mach.h> // macOS 시스템 프로세스 메모리 조회용 Header

@implementation Base {
    id<MTLDevice>  _device;
    CFTimeInterval _lastFrameTimestamp;
    NSInteger      _frameCount;
    CFTimeInterval _accumulatedTime;
} // Base

- (instancetype)initWithDevice:(id<MTLDevice>)device {
    self = [super init];
    if (self) {
        _device = device;
        _lastFrameTimestamp = CACurrentMediaTime();
        _totalTime = 0;
        _deltaTime = 0;
        _fps = 0;
        _frameCount = 0;
        _accumulatedTime = 0;
        _ramUsageMB = 0;
        _vramUsageMB = 0;
        
        if (device != nil) {
            _chipsetName = [device name];
        } else {
            _chipsetName = @"Unknown Chipset";
        }
    }
    return self;
} // initWithDevice

- (void)tick {
    CFTimeInterval currentTime = CACurrentMediaTime();
    _deltaTime = currentTime - _lastFrameTimestamp;
    _lastFrameTimestamp = currentTime;
    _totalTime += _deltaTime;
    
    _accumulatedTime += _deltaTime;
    _frameCount += 1;
    
    // 0.5초 주기로 FPS 및 메모리 사용량 갱신
    // 매 프레임 호출로 인한 부하 방지
    if (_accumulatedTime >= 0.5) {
        _fps = (double)_frameCount / _accumulatedTime;
        _accumulatedTime = 0;
        _frameCount = 0;
        
        [self updateMemoryUsage];
    }
} // tick

- (void)updateMemoryUsage {
    // RAM 사용량 측정 (Resident Memory Size)
    task_basic_info_data_t info;
    mach_msg_type_number_t count = TASK_BASIC_INFO_COUNT;
    if (task_info(mach_task_self(), TASK_BASIC_INFO, (task_info_t)&info, &count) == KERN_SUCCESS) {
        _ramUsageMB = (double)info.resident_size / (1024.0 * 1024.0);
    }
    
    // VRAM 사용량 측정
    // Metal이 현재 버퍼/텍스처 등으로 할당한 총 메모리
    if (_device) {
        _vramUsageMB = (double)_device.currentAllocatedSize / (1024.0 * 1024.0);
    }
} // updateMemoryUsage

@end
