#ifndef _HENGRAPHICS_H_
#define _HENGRAPHICS_H_

#include "vendor/stb/include/stb_image.h" // #define STB_IMAGE_IMPLEMENTATION is in henGraphics.cpp

#include "core/henMath.h"

#include <string>
#include <memory>

namespace hen::graphics
{
    enum class SHADER_PRIMITIVES
    {
        FLOAT,
        FLOAT2,
        FLOAT3,
        FLOAT4,
        INT,
        INT2,
        INT3,
        INT4,
        MAT3,
        MAT4,
        BOOL
    };

    enum class SHADER_TYPES
    {
        PROGRAM,
        VERTEX,
        FRAGMENT
    };

    struct Texture
    {
        int Width;
        int Height;
        int Components;
        unsigned char* Data;
        uint32_t ID = 0;

        void Load(const char* path); // Load from a path
        void Load(const unsigned char* data, int size, int width, int height, int components); // Load from memory

        void Destroy();

        ~Texture();
    };

    struct BufferElement
    {
        std::string Name;
        uint32_t Size;
        size_t Offset;
        SHADER_PRIMITIVES Type;
        bool Normalised;

        BufferElement(SHADER_PRIMITIVES primitive, const std::string& name, bool normalised = false);

        uint32_t GetComponentCount() const;
    };


    class BufferLayout
    {
    public:
        BufferLayout() = default;
        BufferLayout(const std::initializer_list<BufferElement>& elements);

        inline const std::vector<BufferElement>& GetElements() const
        {
            return m_Elements;
        }

        uint32_t GetStride() const
        {
            return m_Stride;
        }

        std::vector<BufferElement>::iterator begin();
        std::vector<BufferElement>::iterator end();
        std::vector<BufferElement>::const_iterator begin() const;
        std::vector<BufferElement>::const_iterator end() const;

    private:
        std::vector<BufferElement> m_Elements;
        uint32_t m_Stride;
    };

    class VertexBuffer
    {
    public:
        virtual ~VertexBuffer() = default;

        virtual void Bind() const = 0;
        virtual void UnBind() const = 0;

        virtual const BufferLayout& GetLayout() const = 0;
        virtual void SetLayout(const BufferLayout& layout) = 0;

        static std::unique_ptr<VertexBuffer> Create(uint32_t size, float* vertices);
    };

    class IndexBuffer
    {
    public:
        virtual ~IndexBuffer() = default;

        virtual void Bind() const = 0;
        virtual void UnBind() const = 0;

        virtual uint32_t GetCount() const = 0;

        static std::unique_ptr<IndexBuffer> Create(uint32_t size, uint32_t* indices);
    };

    class VertexArray
	{
	public:
        bool IsBackendValid() const;

		void Bind() const;
		void UnBind() const;

		void AddVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer);
		void SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer);

		const std::vector<std::shared_ptr<VertexBuffer>>& GetVertexBuffers() const;
		const std::shared_ptr<IndexBuffer>& GetIndexBuffer() const;

		void Create();

    public:
        struct Backend
        {
            virtual ~Backend() = default;

            virtual void Bind() const = 0;
		    virtual void UnBind() const = 0;

		    virtual void AddVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer) = 0;
		    virtual void SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer) = 0;

		    virtual const std::vector<std::shared_ptr<VertexBuffer>>& GetVertexBuffers() const = 0;
		    virtual const std::shared_ptr<IndexBuffer>& GetIndexBuffer() const = 0;
        };

    private:
        std::unique_ptr<Backend> m_BackendImpl;
	};

    class UniformBuffer
    {
    public:
        bool IsBackendValid() const;

        void Create(size_t size, unsigned binding);
        void SetData(const void* data, size_t size, size_t offset = 0);
        size_t GetSize() const;
        unsigned GetBinding();

    public:    
        struct Backend
        {
            virtual ~Backend() = default;

            virtual void SetData(const void* data, size_t size, size_t offset = 0) = 0;
            virtual size_t GetSize() const = 0;
            virtual unsigned GetBinding() = 0;
        };
        
    private:
        std::unique_ptr<Backend> m_BackendImpl;
    };

    class Shader
    {
    public:
        Shader() = default;
        Shader(const char* vsPath, const char* fsPath);

        bool IsBackendValid() const;    

        void Compile();
        void Bind();
        void UnBind();

        uint32_t GetID() const;

        void SetVal(const std::string& name, bool val) const;
        void SetVal(const std::string& name, int val) const;
        void SetVal(const std::string& name, float val) const;

        void SetVec2(const std::string &name, const math::Vec2 &value) const;
        void SetVec3(const std::string &name, const math::Vec3 &value) const;
        void SetVec4(const std::string &name, const math::Vec4 &value) const;

        void SetMat2(const std::string &name, const math::Matrix2 &mat) const;
        void SetMat3(const std::string &name, const math::Matrix3 &mat) const;
        void SetMat4(const std::string &name, const math::Matrix4 &mat) const;

        void Create(const char* vsPath, const char* fsPath);
        void Destroy();

    public:
        struct Backend
        {
            virtual ~Backend() = default;

            virtual void Compile() = 0;
            virtual void Bind() = 0;
            virtual void UnBind() = 0;

            virtual uint32_t GetID() const = 0;

            virtual void SetVal(const std::string& name, bool val) const = 0;
            virtual void SetVal(const std::string& name, int val) const = 0;
            virtual void SetVal(const std::string& name, float val) const = 0;

            virtual void SetVec2(const std::string &name, const math::Vec2 &value) const = 0;
            virtual void SetVec3(const std::string &name, const math::Vec3 &value) const = 0;
            virtual void SetVec4(const std::string &name, const math::Vec4 &value) const = 0;

            virtual void SetMat2(const std::string &name, const math::Matrix2 &mat) const = 0;
            virtual void SetMat3(const std::string &name, const math::Matrix3 &mat) const = 0;
            virtual void SetMat4(const std::string &name, const math::Matrix4 &mat) const = 0;
        };   

    private:
        std::unique_ptr<Backend> m_BackendImpl;
    };

    uint32_t PrimitiveSize(SHADER_PRIMITIVES primitive);

}

#endif // !_HENGRAPHICS_H_