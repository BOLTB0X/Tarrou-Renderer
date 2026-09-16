//
//  ModelLoader.hpp
//  Tarrou-Renderer
//
//  Created by B0X on 9/9/26.
//

#ifndef ModelLoader_hpp
#define ModelLoader_hpp

#include <string>
#include "MeshData.hpp"

namespace ModelLoader {
    // device: id<MTLDevice>를 (__bridge void*)로 받은 포인터
    // path: 파일 경로 (utf-8)
    // returns: 성공 시 true, mesh에 데이터 채움
    bool LoadOBJ(const std::string&, void*, Mesh&);
} // ModelLoader

#endif /* ModelLoader_hpp */
