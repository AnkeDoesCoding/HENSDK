#include "graphics/henGraphics.h"

#include "src/graphics/henGraphics_OpenGL.h"
#include "tools/henConsole.h"
#include "renderer/henRenderer.h"

#include <glad/include/glad.h>

namespace hen::graphics
{
    uint32_t PrimitiveSize(SHADER_PRIMITIVES primitive)
    {
        switch (primitive)
        {
        case SHADER_PRIMITIVES::FLOAT:
            return 4;
            break;
        case SHADER_PRIMITIVES::FLOAT2:
            return 4 * 2;
            break;
        case SHADER_PRIMITIVES::FLOAT3:
            return 4 * 3;
            break;
        case SHADER_PRIMITIVES::FLOAT4:
            return 4 * 4;
            break;
        case SHADER_PRIMITIVES::INT:
            return 4;
            break;
        case SHADER_PRIMITIVES::INT2:
            return 4 * 2;
            break;
        case SHADER_PRIMITIVES::INT3:
            return 4 * 3;
            break;
        case SHADER_PRIMITIVES::INT4:
            return 4 * 4;
            break;
        case SHADER_PRIMITIVES::MAT3:
            return 4 * 3 * 3;
            break;
        case SHADER_PRIMITIVES::MAT4:
            return 4 * 4 * 4;
            break;
        case SHADER_PRIMITIVES::BOOL:
            return 4;
            break;
        default:
            HEN_ASSERT(false, "Failed to get primitive size");
            return 0;
            break;
        }
    }

    Texture::Texture()
    {

    }

    Texture::~Texture()
    {
        if (IsBackendValid())
        {
            m_BackendImpl.reset();
        }

        delete [] Data;
    }

    Texture::Texture(Texture&& other) noexcept
    {
        *this = std::move(other);
    }

    bool Texture::IsBackendValid() const
    {
        return m_BackendImpl != nullptr;
    }

    uint32_t Texture::GetID() const
    {
        if (IsBackendValid())
        {
            return m_BackendImpl->GetID();
        }

        return 0;
    }

    void Texture::Load(const char* path)
    {
        State = RESOURCE_STATES::NOT_READY;

        unsigned char* stbiData = stbi_load(path, &Width, &Height, &Components, 0);

        if (!stbiData)
        {
            HEN_ERROR("[hen::Texture] Failed to load texture from path: " + std::string(path));
            stbi_image_free(stbiData);
            return;
        }

        size_t dataSize = (size_t)(Width * Height * Components);

        delete [] Data;
        Data = new unsigned char[dataSize];

        memcpy(Data, stbiData, dataSize);

        stbi_image_free(stbiData);

        State = RESOURCE_STATES::READY_TO_UPLOAD;

        HEN_LOG("[hen::Texture] Successfully loaded texture from path:" + std::string(path));
    }

    void Texture::Load(std::vector<std::string> faces)
    {
        State = RESOURCE_STATES::NOT_READY;

        CubemapData.resize(6);

        for (uint32_t i = 0; i < faces.size(); i++)
        {
            unsigned char* stbiData = stbi_load(faces[i].c_str(), &Width, &Height, &Components, 0);
            size_t dataSize = (size_t)(Width * Height * Components);

            if (!stbiData)
            {
                HEN_ERROR("[hen::Texture] Failed to load cubemap texture from path: " + std::string(faces[i].c_str()));
                stbi_image_free(stbiData);
                return;
            }

            if (CubemapData[i])
            {
                delete [] CubemapData[i];
            }
            
            CubemapData[i] = new unsigned char[dataSize];

            memcpy(CubemapData[i], stbiData, dataSize);

            stbi_image_free(stbiData);
        }

        State = RESOURCE_STATES::READY_TO_UPLOAD;

        HEN_LOG("[hen::Texture] Successfully loaded cubemap texture");
    }

    void Texture::Load(const unsigned char* data, int size, int width, int height, int components)
    {
        State = RESOURCE_STATES::NOT_READY;

        if (!data)
        {
            HEN_ERROR("[hen::Texture] Failed to load text from address: 0x" + std::to_string(reinterpret_cast<uintptr_t>(data)));
            return;
        }

        delete [] Data;
        Data = new unsigned char[size];

        memcpy(Data, data, size);

        Width = width;
        Height = height;
        Components = components;

        State = RESOURCE_STATES::READY_TO_UPLOAD;

        HEN_LOG("[hen::Texture] Successfully loaded texture from address: 0x" + std::to_string(reinterpret_cast<uintptr_t>(data)));
    }

    void Texture::CreateRenderData()
    {     
        if (!IsBackendValid())
        {
            switch (renderer::CurrentBackend)
            {
                case renderer::BACKEND::OPENGL:
                    m_BackendImpl = std::make_unique<Texture_OpenGL>();
                    break;
                default:
                    m_BackendImpl = nullptr;
                    return;
            }
        }

        if (CubemapData.empty())
        {
            m_BackendImpl->CreateRenderData(Width, Height, Components, Data);
        }
        else
        {
            m_BackendImpl->CreateRenderData(Width, Height, Components, CubemapData);
        }

        State = RESOURCE_STATES::READY_TO_RENDER;
    }

    BufferElement::BufferElement(SHADER_PRIMITIVES primitive, const std::string& name, bool normalised)
        : Name(name), Size(PrimitiveSize(primitive)), Offset(0), Type(primitive), Normalised(normalised)
    {
        
    }

    uint32_t BufferElement::GetComponentCount() const
    {
        switch (Type)
        {
            case SHADER_PRIMITIVES::FLOAT:
                return 1;
                break;
            case SHADER_PRIMITIVES::FLOAT2:
                return 2;
                break;
            case SHADER_PRIMITIVES::FLOAT3:
                return 3;
                break;
            case SHADER_PRIMITIVES::FLOAT4:
                return 4;
                break;
            case SHADER_PRIMITIVES::INT:
                return 1;
                break;
            case SHADER_PRIMITIVES::INT2:
                return 2;
                break;
            case SHADER_PRIMITIVES::INT3:
                return 3;
                break;
            case SHADER_PRIMITIVES::INT4:
                return 4;
                break;
            case SHADER_PRIMITIVES::MAT3:
                return 3;
                break;
            case SHADER_PRIMITIVES::MAT4:
                return 4;
                break;
            case SHADER_PRIMITIVES::BOOL:
                return 1;
                break;
            default:
                HEN_ASSERT(false, "Couldn't get component count");
                return 0;
                break;
        }
    }

    BufferLayout::BufferLayout(const std::initializer_list<BufferElement>& elements) 
        : m_Elements(elements), m_Stride(0)
    {
        size_t offset = 0;
        m_Stride = 0;
        for (graphics::BufferElement& element : m_Elements)
        {
            element.Offset = offset;
            offset += element.Size;
            m_Stride += element.Size;
        }
    }

    std::vector<BufferElement>::iterator BufferLayout::begin() 
    {
        return m_Elements.begin();
    }

    std::vector<BufferElement>::iterator BufferLayout::end() 
    {
        return m_Elements.end();
    }

    std::vector<BufferElement>::const_iterator BufferLayout::begin() const 
    {
        return m_Elements.begin();
    }

    std::vector<BufferElement>::const_iterator BufferLayout::end() const 
    {
        return m_Elements.end();
    }

    std::unique_ptr<VertexBuffer> VertexBuffer::Create(uint32_t size, float* vertices)
    {
        switch (renderer::CurrentBackend)
        {
            case renderer::BACKEND::OPENGL:
                return std::make_unique<VertexBuffer_OpenGL>(size, vertices);
                break;
            default:
                return nullptr;
                break;
        }
    }

    std::unique_ptr<IndexBuffer> IndexBuffer::Create(uint32_t size, uint32_t* count)
    {
        switch (renderer::CurrentBackend)
        {
            case renderer::BACKEND::OPENGL:
                return std::make_unique<IndexBuffer_OpenGL>(size, count);
                break;
            default:
                return nullptr;
                break;
        }
    }

    bool VertexArray::IsBackendValid() const
    {
        return m_BackendImpl != nullptr;
    }

    void VertexArray::Bind() const
    {
        if (IsBackendValid())
        {
            m_BackendImpl->Bind();
        }
    }

    void VertexArray::UnBind() const
    {
        if (IsBackendValid())
        {
            m_BackendImpl->UnBind();
        }
    }

    void VertexArray::AddVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer)
    {
        if (IsBackendValid())
        {
            m_BackendImpl->AddVertexBuffer(vertexBuffer);
        }
    }

    void VertexArray::SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer)
    {
        if (IsBackendValid())
        {
            m_BackendImpl->SetIndexBuffer(indexBuffer);
        }
    }

    const std::vector<std::shared_ptr<VertexBuffer>>& VertexArray::GetVertexBuffers() const
    {
        static const std::vector<std::shared_ptr<VertexBuffer>> empty;

        if (IsBackendValid())
        {
            return m_BackendImpl->GetVertexBuffers();
        }

        return empty;
    }

    const std::shared_ptr<IndexBuffer>& VertexArray::GetIndexBuffer() const
    {
        static const std::shared_ptr<IndexBuffer> empty;

        if (IsBackendValid())
        {
            return m_BackendImpl->GetIndexBuffer();
        }

        return empty;
    }

    void VertexArray::Create()
    {
        switch (renderer::CurrentBackend)
        {
            case renderer::BACKEND::OPENGL:
                m_BackendImpl = std::make_unique<VertexArray_OpenGL>();
                break;
            default:
                m_BackendImpl = nullptr;
                break;
        }
    }

    bool UniformBuffer::IsBackendValid() const
    {
        return m_BackendImpl != nullptr;
    }

    void UniformBuffer::Create(size_t size, unsigned binding)
    {
        switch (renderer::CurrentBackend)
        {
            case renderer::BACKEND::OPENGL:
                m_BackendImpl = std::make_unique<UniformBuffer_OpenGL>(size, binding);
                break;
            default:
                m_BackendImpl = nullptr;
                break;
        }
    }

    void UniformBuffer::SetData(const void* data, size_t size, size_t offset)
    {
        if (IsBackendValid())
        {
            m_BackendImpl->SetData(data, size, offset);
        }
    }

    size_t UniformBuffer::GetSize() const
    {
        if (IsBackendValid())
        {
            return m_BackendImpl->GetSize();
        }
        
        return 0;
    }

    unsigned UniformBuffer::GetBinding()
    {
        if (IsBackendValid())
        {
            return m_BackendImpl->GetBinding();
        }

        return -1;
    }

    Shader::Shader(const char* vsPath, const char* fsPath)
    {
        Create(vsPath, fsPath);
    }

    bool Shader::IsBackendValid() const
    {
        return m_BackendImpl != nullptr;
    }

    void Shader::Compile()
    {
        if (IsBackendValid())
        {
            m_BackendImpl->Compile();
        }
    }

    void Shader::Bind()
    {
        if (IsBackendValid())
        {
            m_BackendImpl->Bind();
        }
    }

    void Shader::UnBind()
    {
        if (IsBackendValid())
        {
            m_BackendImpl->UnBind();
        }
    }

    uint32_t Shader::GetID() const
    {
        if (IsBackendValid())
        {
            return m_BackendImpl->GetID();
        }

        return 0;
    }

    void Shader::SetVal(const std::string& name, bool val) const
    {
        if (IsBackendValid())
        {
            m_BackendImpl->SetVal(name, val);
        }
    }

    void Shader::SetVal(const std::string& name, int val) const
    {
        if (IsBackendValid())
        {
            m_BackendImpl->SetVal(name, val);
        }
    }

    void Shader::SetVal(const std::string& name, float val) const
    {
        if (IsBackendValid())
        {
            m_BackendImpl->SetVal(name, val);
        }
    }

    void Shader::SetVec2(const std::string &name, const math::Vec2 &value) const
    {
        if (IsBackendValid())
        {
            m_BackendImpl->SetVec2(name, value);
        }
    }

    void Shader::SetVec3(const std::string &name, const math::Vec3 &value) const
    {
        if (IsBackendValid())
        {
            m_BackendImpl->SetVec3(name, value);
        }
    }

    void Shader::SetVec4(const std::string &name, const math::Vec4 &value) const
    {
        if (IsBackendValid())
        {
            m_BackendImpl->SetVec4(name, value);
        }
    }

    void Shader::SetMat2(const std::string &name, const math::Matrix2 &mat) const
    {   
        if (IsBackendValid())
        {
            m_BackendImpl->SetMat2(name, mat);
        }
    }

    void Shader::SetMat3(const std::string &name, const math::Matrix3 &mat) const
    {
        if (IsBackendValid())
        {
            m_BackendImpl->SetMat3(name, mat);
        }
    }

    void Shader::SetMat4(const std::string &name, const math::Matrix4 &mat) const
    {
        if (IsBackendValid())
        {
            m_BackendImpl->SetMat4(name, mat);
        }
    }   

    void Shader::Create(const char *vsPath, const char *fsPath)
    {
        switch (renderer::CurrentBackend)
        {
            case renderer::BACKEND::OPENGL:
                m_BackendImpl = std::make_unique<Shader_OpenGL>(vsPath, fsPath);
                HEN_LOG("[hen::Shader] Successfully loaded shader from paths: " + std::string(vsPath) + " / " + std::string(fsPath));
                break;
            default:
                m_BackendImpl = nullptr;
                break;
        }
    }

    void Shader::Destroy()
    {
        if (IsBackendValid())
        {
            m_BackendImpl.reset();
        }
    }

}