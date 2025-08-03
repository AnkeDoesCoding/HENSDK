#ifndef _HENGRAPHICS_OPENGL_H_
#define _HENGRAPHICS_OPENGL_H_

#include "graphics/henGraphics.h"

namespace hen::graphics
{
    class VertexBuffer_OpenGL : public VertexBuffer
    {
    public:
        VertexBuffer_OpenGL(uint32_t size, float* vertices);
        ~VertexBuffer_OpenGL() override;

        void Bind() const override;
        void UnBind() const override;

    private:
        unsigned int m_ID;
    };

    class IndexBuffer_OpenGL : public IndexBuffer
    {
    public:
        IndexBuffer_OpenGL(uint32_t count, uint32_t* indices);
        ~IndexBuffer_OpenGL() override;

        uint32_t GetCount() const override;

        void Bind() const override;
        void UnBind() const override;

    private:
        unsigned int m_ID;
        uint32_t m_Count;
    };
}

#endif // !_HENGRAPHICS_OPENGL_H_