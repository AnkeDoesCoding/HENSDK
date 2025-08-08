#ifndef _HENGRAPHICS_H_
#define _HENGRAPHICS_H_

#include "vendor/glm/glm.hpp"
#include "vendor/glm/gtc/matrix_transform.hpp"

#include <string>
#include <memory>

namespace hen::graphics
{
    class VertexBuffer
    {
    public:
        virtual ~VertexBuffer() = default;

        virtual void Bind() const = 0;
        virtual void UnBind() const = 0;

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

    class Shader
    {
    public:

        virtual void Activate() = 0;
        virtual void Run() = 0;

        virtual unsigned int GetID() = 0;

        virtual void SetVal(const std::string& name, bool val) const = 0;
        virtual void SetVal(const std::string& name, int val) const = 0;
        virtual void SetVal(const std::string& name, float val) const = 0;

        virtual void SetVec2(const std::string &name, const glm::vec2 &value) const = 0;
        virtual void SetVec3(const std::string &name, const glm::vec3 &value) const = 0;
        virtual void SetVec4(const std::string &name, const glm::vec4 &value) const = 0;

        virtual void SetMat2(const std::string &name, const glm::mat2 &mat) const = 0;
        virtual void SetMat3(const std::string &name, const glm::mat3 &mat) const = 0;
        virtual void SetMat4(const std::string &name, const glm::mat4 &mat) const = 0;

        static std::unique_ptr<Shader> Create(const char* vsPath, const char* fsPath);
    };
}

#endif // !_HENGRAPHICS_H_