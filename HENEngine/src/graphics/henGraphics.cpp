#include "graphics/henGraphics.h"

#include "src/graphics/henGraphics_OpenGL.h"
#include "tools/henConsole.h"
#include "renderer/henRenderer.h"

namespace hen::graphics
{
    VertexBuffer* VertexBuffer::Create(uint32_t size, float* vertices)
    {
        switch(renderer::CurrentBackend)
        {
            case renderer::BACKEND::NONE:
                console::Post("[hen::renderer] BACKEND::NONE doesn't exist", console::LOGLEVEL::ERROR);
                return nullptr;
            case renderer::BACKEND::OPENGL:
                return new VertexBuffer_OpenGL(size, vertices);
        }

        return nullptr;
    }

    IndexBuffer* IndexBuffer::Create(uint32_t size, uint32_t* count)
    {
        switch(renderer::CurrentBackend)
        {
            case renderer::BACKEND::NONE:
                console::Post("[hen::renderer] BACKEND::NONE doesn't exist", console::LOGLEVEL::ERROR);
                return nullptr;
            case renderer::BACKEND::OPENGL:
                return new IndexBuffer_OpenGL(size, count);
        }

        return nullptr;
    }

    Shader *Shader::Create(const char *vsPath, const char *fsPath)
    {
        switch(renderer::CurrentBackend)
        {
            case renderer::BACKEND::NONE:
                console::Post("[hen::renderer] BACKEND::NONE doesn't exist", console::LOGLEVEL::ERROR);
                return nullptr;
            case renderer::BACKEND::OPENGL:
                return new Shader_OpenGL(vsPath, fsPath);
        }

        return nullptr;
    }
}