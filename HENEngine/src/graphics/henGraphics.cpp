#define STB_IMAGE_IMPLEMENTATION
#include "graphics/henGraphics.h"

#include "src/graphics/henGraphics_OpenGL.h"
#include "tools/henConsole.h"
#include "renderer/henRenderer.h"


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
        }

        console::Log("[hen::graphics] Failed to get primitive size", console::LOGLEVEL::ERROR);
        return 0;
    }

    void Texture::Load(const char* path)
    {
        glGenTextures(1, &ID);

        Data = stbi_load(path, &Width, &Height, &Components, 0);
        if (Data)
        {
            GLenum format;
            if (Components == 1)
            {
                format = GL_RED;
            }
            else if (Components == 3)
            {
                format = GL_RGB;
            }
            else if (Components == 4)
            {
                format = GL_RGBA;
            }

            glBindTexture(GL_TEXTURE_2D, ID);
            glTexImage2D(GL_TEXTURE_2D, 0, format, Width, Height, 0, format, GL_UNSIGNED_BYTE, Data);
            glGenerateMipmap(GL_TEXTURE_2D);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            Destroy();

        }
        else
        {
            console::Log("Failed to load texture", console::LOGLEVEL::ERROR);
            
            Destroy();
        }
    }

    Texture::~Texture()
    {
        Destroy();
    }

    void Texture::Destroy()
    {
        if (Data)
        {
            stbi_image_free(Data);
            Data = nullptr;
        }
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
        }

        console::Log("[hen::graphics] Couldn't get component count", console::LOGLEVEL::ERROR);
        return 0;
    }

    BufferLayout::BufferLayout(const std::initializer_list<BufferElement>& elements) 
        : m_Elements(elements), m_Stride(0)
    {
        size_t offset = 0;
        m_Stride = 0;
        for (auto& element : m_Elements)
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
            case renderer::BACKEND::NONE:
                console::Log("[hen::renderer] BACKEND::NONE doesn't exist", console::LOGLEVEL::ERROR);
                return nullptr;
                break;
            case renderer::BACKEND::OPENGL:
                return std::make_unique<VertexBuffer_OpenGL>(size, vertices);
                break;
        }

        return nullptr;
    }

    std::unique_ptr<IndexBuffer> IndexBuffer::Create(uint32_t size, uint32_t* count)
    {
        switch (renderer::CurrentBackend)
        {
            case renderer::BACKEND::NONE:
                console::Log("[hen::renderer] BACKEND::NONE doesn't exist", console::LOGLEVEL::ERROR);
                return nullptr;
                break;
            case renderer::BACKEND::OPENGL:
                return std::make_unique<IndexBuffer_OpenGL>(size, count);
                break;
        }

        return nullptr;
    }

    std::unique_ptr<VertexArray> VertexArray::Create()
    {
        switch (renderer::CurrentBackend)
        {
            case renderer::BACKEND::NONE:
                console::Log("[hen::renderer] BACKEND::NONE doesn't exist", console::LOGLEVEL::ERROR);
                return nullptr;
                break;
            case renderer::BACKEND::OPENGL:
                return std::make_unique<VertexArray_OpenGL>();
                break;
        }

        return nullptr;
    }

    Shader::Shader(const char* vsPath, const char* fsPath)
    {
        Create(vsPath, fsPath);
    }

    Shader::~Shader()
    {
        Destroy();
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

    unsigned int Shader::GetID() const
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

    void Shader::SetVec2(const std::string &name, const glm::vec2 &value) const
    {
        if (IsBackendValid())
        {
            m_BackendImpl->SetVec2(name, value);
        }
    }

    void Shader::SetVec3(const std::string &name, const glm::vec3 &value) const
    {
        if (IsBackendValid())
        {
            m_BackendImpl->SetVec3(name, value);
        }
    }

    void Shader::SetVec4(const std::string &name, const glm::vec4 &value) const
    {
        if (IsBackendValid())
        {
            m_BackendImpl->SetVec4(name, value);
        }
    }

    void Shader::SetMat2(const std::string &name, const glm::mat2 &mat) const
    {   
        if (IsBackendValid())
        {
            m_BackendImpl->SetMat2(name, mat);
        }
    }

    void Shader::SetMat3(const std::string &name, const glm::mat3 &mat) const
    {
        if (IsBackendValid())
        {
            m_BackendImpl->SetMat3(name, mat);
        }
    }

    void Shader::SetMat4(const std::string &name, const glm::mat4 &mat) const
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
            case renderer::BACKEND::NONE:
                console::Log("[hen::renderer] BACKEND::NONE doesn't exist", console::LOGLEVEL::ERROR);
                m_BackendImpl = nullptr;
                break;
            case renderer::BACKEND::OPENGL:
                m_BackendImpl = std::make_unique<Shader_OpenGL>(vsPath, fsPath);
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