#include "ModelLoader.hpp"
#include "ModelLoaderBridge.h"
#include <string>

bool ModelLoader::LoadOBJ(const std::string& path, void* devicePtr, Mesh& meshOut) {
    return ModelLoaderBridge_LoadOBJ(path.c_str(), devicePtr, static_cast<void*>(&meshOut));
} // LoadOBJ
