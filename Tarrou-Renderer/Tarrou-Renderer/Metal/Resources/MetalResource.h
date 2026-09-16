//
//  MetalResource.h
//  Tarrou-Renderer
//
//  Created by B0X on 9/16/26.
//

#ifndef MetalResource_h
#define MetalResource_h

class MetalResource {
public:
    MetalResource() = default;
    ~MetalResource();

    MetalResource(const MetalResource&) = delete;
    MetalResource& operator=(const MetalResource&) = delete;

    MetalResource(MetalResource&& other) noexcept;
    MetalResource& operator=(MetalResource&& other) noexcept;

    static MetalResource Adopt(void*);
    void                 Reset();
    void*                Get() const { return m_handle; }

    explicit operator bool() const { return m_handle != nullptr; }

private:
    explicit MetalResource(void* handle);
    void     Release();

private:
    void* m_handle = nullptr;
}; // MetalResource

#endif /* MetalResource_h */
