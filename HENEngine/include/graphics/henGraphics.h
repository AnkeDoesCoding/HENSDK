#ifndef _HENGRAPHICS_H_
#define _HENGRAPHICS_H_

#include "graphics/henGraphics_Shader.h"

namespace hen::graphics
{
    class VertexBuffer
    {
    public:
        virtual ~VertexBuffer() = default;

        virtual void Bind() const = 0;
        virtual void UnBind() const = 0;

        static VertexBuffer* Create(uint32_t size, float* vertices);
    };

    class IndexBuffer
    {
    public:
        virtual ~IndexBuffer() = default;

        virtual void Bind() const = 0;
        virtual void UnBind() const = 0;

        virtual uint32_t GetCount() const = 0;

        static IndexBuffer* Create(uint32_t size, uint32_t* indices);
    };
}

#endif // !_HENGRAPHICS_H_