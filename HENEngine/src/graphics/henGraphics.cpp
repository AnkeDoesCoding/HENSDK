#include "graphics/henGraphics.h"

#include "src/graphics/henGraphics_OpenGL.h"
#include "tools/henConsole.h"
#include "renderer/henRenderer.h"


namespace hen::graphics
{
    uint32_t GetShaderPrimitiveSize(SHADER_PRIMITIVES primitive)
    {
        switch (primitive)
        {
        case SHADER_PRIMITIVES::FLOAT:
            return 4;
        case SHADER_PRIMITIVES::FLOAT2:
            return 8;
        case SHADER_PRIMITIVES::FLOAT3:
            return 12;
        case SHADER_PRIMITIVES::FLOAT4:
            return 16;
        case SHADER_PRIMITIVES::INT:
            return 4;
        case SHADER_PRIMITIVES::INT2:
            return 8;
        case SHADER_PRIMITIVES::INT3:
            return 12;
        case SHADER_PRIMITIVES::INT4:
            return 16;
        case SHADER_PRIMITIVES::MAT3:
            return 36;
        case SHADER_PRIMITIVES::MAT4:
            return 64;
        case SHADER_PRIMITIVES::BOOL:
            return 4;
        default:
            break;
        }
    }

    std::unique_ptr<VertexBuffer> VertexBuffer::Create(uint32_t size, float* vertices)
    {
        switch(renderer::CurrentBackend)
        {
            case renderer::BACKEND::NONE:
                console::Post("[hen::renderer] BACKEND::NONE doesn't exist", console::LOGLEVEL::ERROR);
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
                console::Post("[hen::renderer] BACKEND::NONE doesn't exist", console::LOGLEVEL::ERROR);
                return nullptr;
            case renderer::BACKEND::OPENGL:
                return std::make_unique<IndexBuffer_OpenGL>(size, count);
        }

        return nullptr;
    }

    std::unique_ptr<Shader> Shader::Create(const char *vsPath, const char *fsPath)
    {
        switch(renderer::CurrentBackend)
        {
            case renderer::BACKEND::NONE:
                console::Post("[hen::renderer] BACKEND::NONE doesn't exist", console::LOGLEVEL::ERROR);
                return nullptr;
            case renderer::BACKEND::OPENGL:
                return std::make_unique<Shader_OpenGL>(vsPath, fsPath);
        }

        return nullptr;
    }
}