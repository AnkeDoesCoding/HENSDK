#ifndef _HENGRAPHICS_OPENGL_H_
#define _HENGRAPHICS_OPENGL_H_

#include "graphics/henGraphics.h"

namespace hen::graphics
{
    class Texture_OpenGL : public Texture::Backend
    {
    public:
        ~Texture_OpenGL();

        uint32_t GetID() const override;

        void CreateRenderData(const TextureDesc& textureDesc, const unsigned char* data) override;
        void CreateRenderData(const TextureDesc& textureDesc, std::vector<unsigned char*> cubemapData) override;

    private:
        uint32_t m_ID;
    };

    class Buffer_OpenGL : public Buffer::Backend
    {
    public:
        void Create(size_t size, float* vertices) override;
        void Create(uint32_t count, uint32_t* indices) override;
        void Create(size_t size, uint32_t binding) override;

        void Bind() const override;
        void UnBind() const override;

        const BUFFER_TYPES GetType() const override;
        const uint32_t GetID() const override;
        const uint32_t GetCount() const override;
        const uint32_t GetBinding() const override;
        const size_t GetSize() const override;
        const BufferLayout GetLayout() const override;

        void SetLayout(const BufferLayout& layout) override;
        void SetData(const void* data, size_t size, size_t offset) override;
    
    private:
        BUFFER_TYPES m_Type = BUFFER_TYPES::NONE;
        uint32_t m_ID;

        BufferLayout m_Layout;
        uint32_t m_Count;
        size_t m_Size;
        uint32_t m_Binding;
    };

    class VertexBuffer_OpenGL : public VertexBuffer
    {
    public:
        VertexBuffer_OpenGL(uint32_t size, float* vertices);
        ~VertexBuffer_OpenGL() override;

        uint32_t GetID() const override;

        void Bind() const override;
        void UnBind() const override;

        const BufferLayout& GetLayout() const override;
        void SetLayout(const BufferLayout& layout) override;

    private:
        uint32_t m_ID;

        BufferLayout m_Layout;
    };

    class IndexBuffer_OpenGL : public IndexBuffer
    {
    public:
        IndexBuffer_OpenGL(uint32_t count, uint32_t* indices);
        ~IndexBuffer_OpenGL() override;

        uint32_t GetCount() const override;
        uint32_t GetID() const override;

        void Bind() const override;
        void UnBind() const override;

    private:
        uint32_t m_ID;
        uint32_t m_Count;
    };

    class NewVertexArray_OpenGL : public NewVertexArray::Backend
    {
    public:
        NewVertexArray_OpenGL();
        ~NewVertexArray_OpenGL();

		void Bind() const override;
		void UnBind() const override;

        void AddVertexBuffer(Buffer* vertexBuffer) override;
		void SetIndexBuffer(Buffer* indexBuffer) override;

        const std::vector<Buffer*>& GetVertexBuffers() const override;
		const Buffer* GetIndexBuffer() const override;

    private:
        std::vector<Buffer*> m_VertexBuffers;
        Buffer* m_IndexBuffer;
        uint32_t m_ID;
        uint32_t m_VertexBufferIndex = 0;
    };


    class VertexArray_OpenGL : public VertexArray::Backend
    {
    public:
        VertexArray_OpenGL();
        ~VertexArray_OpenGL();

		void Bind() const override;
		void UnBind() const override;

        void AddVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer) override;
		void SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer) override;

        const std::vector<std::shared_ptr<VertexBuffer>>& GetVertexBuffers() const override;
		const std::shared_ptr<IndexBuffer>& GetIndexBuffer() const override;

    private:
        std::vector<std::shared_ptr<VertexBuffer>> m_VertexBuffers;
        std::shared_ptr<IndexBuffer> m_IndexBuffer;
        uint32_t m_ID;
        uint32_t m_VertexBufferIndex = 0;
    };

    class UniformBuffer_OpenGL : public UniformBuffer::Backend
    {
    public: 
        UniformBuffer_OpenGL(size_t size, unsigned binding);

        void SetData(const void* data, size_t size, size_t offset) override;
        size_t GetSize() const override;
        unsigned GetBinding() override;

    private:
        uint32_t m_ID;
        size_t m_Size;
        unsigned m_Binding;
    };

    class Shader_OpenGL : public Shader::Backend
    {
    public:
        Shader_OpenGL(const char* vsPath, const char* fsPath);

        void Compile() override;
        void Bind() override;
        void UnBind() override;

        uint32_t GetID() const override;

        void SetVal(const std::string& name, bool val) const override;
        void SetVal(const std::string& name, int val) const override;
        void SetVal(const std::string& name, float val) const override;

        void SetVec2(const std::string &name, const math::Vec2 &value) const override;
        void SetVec3(const std::string &name, const math::Vec3 &value) const override;
        void SetVec4(const std::string &name, const math::Vec4 &value) const override;

        void SetMat2(const std::string &name, const math::Matrix2 &mat) const override;
        void SetMat3(const std::string &name, const math::Matrix3 &mat) const override;
        void SetMat4(const std::string &name, const math::Matrix4 &mat) const override;

    private:
        void CheckForCompileErrors(uint32_t shader, SHADER_STAGES type);

    private:
        uint32_t m_ID;
        const char* m_VSPath;
        const char* m_FSPath;
    };
}

#endif // !_HENGRAPHICS_OPENGL_H_