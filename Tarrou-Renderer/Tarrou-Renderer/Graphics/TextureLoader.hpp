//
//  TextureLoader.hpp
//  Tarrou-Renderer
//
//  Created by B0X on 9/15/26.
//

#ifndef TextureLoader_hpp
#define TextureLoader_hpp

#include <memory>
#include <string>

class Texture;

class TextureLoader {
public:
    explicit TextureLoader(void*);
    std::shared_ptr<Texture> Load(const std::string&);

private:
    void* m_Device = nullptr;
};

#endif /* TextureLoader_hpp */
