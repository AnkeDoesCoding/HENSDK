#include "graphics/henGraphics_OpenGL.h"

#include "vendor/glad/include/glad.h"

namespace hen::graphics
{
    VertexBuffer_OpenGL::VertexBuffer_OpenGL(uint32_t size, float* vertices)
    {
        glCreateBuffers(1, &m_ID);
        glBindBuffer(GL_ARRAY_BUFFER, m_ID);
        glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
    }

    VertexBuffer_OpenGL::~VertexBuffer_OpenGL()
    {
        glDeleteBuffers(1, &m_ID);
    }

    void VertexBuffer_OpenGL::Bind() const
    {
        glBindBuffer(GL_ARRAY_BUFFER, m_ID);
    }

    void VertexBuffer_OpenGL::UnBind() const
    {  
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }   

    IndexBuffer_OpenGL::IndexBuffer_OpenGL(uint32_t count, uint32_t* indices)
    {
        m_Count = count;
        glCreateBuffers(1, &m_ID);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ID);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t), indices, GL_STATIC_DRAW);
    }

    IndexBuffer_OpenGL::~IndexBuffer_OpenGL()
    {
        glDeleteBuffers(1, &m_ID);
    }

    uint32_t IndexBuffer_OpenGL::GetCount() const
    {
        return m_Count;
    }

    void IndexBuffer_OpenGL::Bind() const
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ID);
    }

    void IndexBuffer_OpenGL::UnBind() const
    {  
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }   
}