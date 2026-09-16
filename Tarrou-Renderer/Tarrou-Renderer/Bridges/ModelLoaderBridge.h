//
//  ModelLoaderBridge.h
//  Tarrou-Renderer
//
//  Created by B0X on 9/9/26.
//

#ifndef ModelLoaderBridge_h
#define ModelLoaderBridge_h

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

bool ModelLoaderBridge_LoadOBJ(const char*, void*, void*);

#ifdef __cplusplus
}
#endif

#endif /* ModelLoaderBridge_h */
