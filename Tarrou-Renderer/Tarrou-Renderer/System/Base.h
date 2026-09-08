//
//  Base.h
//  Tarrou-Renderer
//
//  Created by B0X on 9/8/26.
//

#import <Foundation/Foundation.h>
#import <Metal/Metal.h>

@interface Base : NSObject

@property (nonatomic, readonly) double deltaTime;            // 프레임 간 소요 시간 (초)
@property (nonatomic, readonly) double fps;                  // 현재 FPS
@property (nonatomic, readonly) double totalTime;            // 총 앱 누적 실행 시간 (초)
@property (nonatomic, readonly, copy) NSString *chipsetName; // GPU 칩셋 이름

@property (nonatomic, readonly) double ramUsageMB;           // 현재 프로세스 RAM 사용량 (MB)
@property (nonatomic, readonly) double vramUsageMB;          // Metal 리소스 VRAM 할당량 (MB)

- (instancetype)initWithDevice:(id<MTLDevice>)device;
- (void)tick; // 매 프레임 호출되어 시간, FPS 및 메모리 지표 갱신

@end
