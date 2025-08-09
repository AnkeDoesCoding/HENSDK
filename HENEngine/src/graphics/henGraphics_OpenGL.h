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

        const BufferLayout& GetLayout() const override;
        void SetLayout(const BufferLayout& layout) override;

    private:
        unsigned int m_ID;

        BufferLayout m_Layout;
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

    class VertexArray_OpenGL : public VertexArray
    {
    public:
        VertexArray_OpenGL();
        ~VertexArray_OpenGL() override;

		void Bind() const override;
		void UnBind() const override;

        void AddVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer) override;
		void SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer) override;

        const std::vector<std::shared_ptr<VertexBuffer>>& GetVertexBuffers() const override;
		const std::shared_ptr<IndexBuffer>& GetIndexBuffer() const override;

    private:
        std::vector<std::shared_ptr<VertexBuffer>> m_VertexBuffers;
        std::shared_ptr<IndexBuffer> m_IndexBuffer;
        unsigned int m_ID;
        uint32_t m_VertexBufferIndex = 0;
    };

    class Shader_OpenGL : public Shader
    {
    public:

        Shader_OpenGL(const char* vsPath, const char* fsPath);

        void Activate() override;
        void Run() override;

        unsigned int GetID() const override;

        void SetVal(const std::string& name, bool val) const override;
        void SetVal(const std::string& name, int val) const override;
        void SetVal(const std::string& name, float val) const override;

        void SetVec2(const std::string &name, const glm::vec2 &value) const override;
        void SetVec3(const std::string &name, const glm::vec3 &value) const override;
        void SetVec4(const std::string &name, const glm::vec4 &value) const override;

        void SetMat2(const std::string &name, const glm::mat2 &mat) const override;
        void SetMat3(const std::string &name, const glm::mat3 &mat) const override;
        void SetMat4(const std::string &name, const glm::mat4 &mat) const override;

    private:
        void CheckForCompileErrors(unsigned int shader, std::string type);

    private:
        unsigned int m_ID;
        const char* m_VSPath;
        const char* m_FSPath;
    };
}

#endif // !_HENGRAPHICS_OPENGL_H_