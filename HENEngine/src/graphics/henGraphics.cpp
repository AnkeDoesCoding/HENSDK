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
        case SHADER_PRIMITIVES::FLOAT2:
            return 4 * 2;
        case SHADER_PRIMITIVES::FLOAT3:
            return 4 * 3;
        case SHADER_PRIMITIVES::FLOAT4:
            return 4 * 4;
        case SHADER_PRIMITIVES::INT:
            return 4;
        case SHADER_PRIMITIVES::INT2:
            return 4 * 2;
        case SHADER_PRIMITIVES::INT3:
            return 4 * 3;
        case SHADER_PRIMITIVES::INT4:
            return 4 * 4;
        case SHADER_PRIMITIVES::MAT3:
            return 4 * 3 * 3;
        case SHADER_PRIMITIVES::MAT4:
            return 4 * 4 * 4;
        case SHADER_PRIMITIVES::BOOL:
            return 4;
        }

        console::Log("[hen::graphics] Failed to get primitive size", console::LOGLEVEL::ERROR);
        return 0;
    }

    BufferElement::BufferElement(SHADER_PRIMITIVES primitive, const std::string& name, bool normalised)
        : Name(name), Size(PrimitiveSize(primitive)), Offset(0), Type(primitive), Normalised(normalised)
    {
        
    }

    uint32_t BufferElement::GetComponentCount() const
    {
        switch(Type)
        {
            case SHADER_PRIMITIVES::FLOAT:
                return 1;
            case SHADER_PRIMITIVES::FLOAT2:
                return 2;
            case SHADER_PRIMITIVES::FLOAT3:
                return 3;
            case SHADER_PRIMITIVES::FLOAT4:
                return 4;
            case SHADER_PRIMITIVES::INT:
                return 1;
            case SHADER_PRIMITIVES::INT2:
                return 2;
            case SHADER_PRIMITIVES::INT3:
                return 3;
            case SHADER_PRIMITIVES::INT4:
                return 4;
            case SHADER_PRIMITIVES::MAT3:
                return 3;
            case SHADER_PRIMITIVES::MAT4:
                return 4;
            case SHADER_PRIMITIVES::BOOL:
                return 1;
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
        switch(renderer::CurrentBackend)
        {
            case renderer::BACKEND::NONE:
                console::Log("[hen::renderer] BACKEND::NONE doesn't exist", console::LOGLEVEL::ERROR);
                return nullptr;
            case renderer::BACKEND::OPENGL:
                return std::make_unique<VertexBuffer_OpenGL>(size, vertices);
        }

        return nullptr;
    }

    std::unique_ptr<IndexBuffer> IndexBuffer::Create(uint32_t size, uint32_t* count)
    {
        switch(renderer::CurrentBackend)
        {
            case renderer::BACKEND::NONE:
                console::Log("[hen::renderer] BACKEND::NONE doesn't exist", console::LOGLEVEL::ERROR);
                return nullptr;
            case renderer::BACKEND::OPENGL:
                return std::make_unique<IndexBuffer_OpenGL>(size, count);
        }

        return nullptr;
    }

    std::unique_ptr<VertexArray> VertexArray::Create()
    {
        switch(renderer::CurrentBackend)
        {
            case renderer::BACKEND::NONE:
                console::Log("[hen::renderer] BACKEND::NONE doesn't exist", console::LOGLEVEL::ERROR);
                return nullptr;
            case renderer::BACKEND::OPENGL:
                return std::make_unique<VertexArray_OpenGL>();
        }

        return nullptr;
    }

    std::unique_ptr<Shader> Shader::Create(const char *vsPath, const char *fsPath)
    {
        switch(renderer::CurrentBackend)
        {
            case renderer::BACKEND::NONE:
                console::Log("[hen::renderer] BACKEND::NONE doesn't exist", console::LOGLEVEL::ERROR);
                return nullptr;
            case renderer::BACKEND::OPENGL:
                return std::make_unique<Shader_OpenGL>(vsPath, fsPath);
        }

        return nullptr;
    }
}