#include "graphics/henGraphics.h"

#include "src/graphics/henGraphics_OpenGL.h"
#include "tools/henConsole.h"
#include "renderer/henRenderer.h"

namespace hen::graphics
{
    VertexBuffer* VertexBuffer::Create(uint32_t size, float* vertices)
    {
        switch(renderer::RendererAPI)
        {
            case RENDERER_API::NONE:
                console::Post("[hen::renderer] RENDERER_API::NONE doesn't exist", console::LOGLEVEL::ERROR);
                return nullptr;
            case RENDERER_API::OPENGL:
                return new VertexBuffer_OpenGL(size, vertices);
        }

        return nullptr;
    }

    IndexBuffer* IndexBuffer::Create(uint32_t size, uint32_t* count)
    {
        switch(renderer::RendererAPI)
        {
            case RENDERER_API::NONE:
                console::Post("[hen::renderer] RENDERER_API::NONE doesn't exist", console::LOGLEVEL::ERROR);
                return nullptr;
            case RENDERER_API::OPENGL:
                return new IndexBuffer_OpenGL(size, count);
        }

        return nullptr;
    }

    Shader *Shader::Create(const char *vsPath, const char *fsPath)
    {
        switch(renderer::RendererAPI)
        {
            case RENDERER_API::NONE:
                console::Post("[hen::renderer] RENDERER_API::NONE doesn't exist", console::LOGLEVEL::ERROR);
                return nullptr;
            case RENDERER_API::OPENGL:
                return new Shader_OpenGL(vsPath, fsPath);
        }

        return nullptr;
    }
}