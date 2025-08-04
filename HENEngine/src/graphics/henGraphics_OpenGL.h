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

    class Shader_OpenGL : public Shader
    {
    public:

        Shader_OpenGL(const char* vsPath, const char* fsPath);

        void Activate() override;
        void Run() override;

        unsigned int GetID() override;

        void SetVal(const std::string& name, bool val) const override;
        void SetVal(const std::string& name, int val) const override;
        void SetVal(const std::string& name, float val) const override;

        void SetVec2(const std::string &name, const glm::vec2 &value) const override;
        void SetVec3(const std::string &name, const glm::vec3 &value) const override;
        void SetVec4(const std::string &name, const glm::vec4 &value) const override;

        void SetMat2(const std::string &name, const glm::mat2 &mat) const override;
        void SetMat3(const std::string &name, const glm::mat3 &mat) const override;
        void SetMat4(const std::string &name, const glm::mat4 &mat) const override;
    public:

        unsigned int m_ID;
    private:

        void CheckForCompileErrors(unsigned int shader, std::string type);

    private:
    
        const char* m_VSPath;
        const char* m_FSPath;
    };
}

#endif // !_HENGRAPHICS_OPENGL_H_