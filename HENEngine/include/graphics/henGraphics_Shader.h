#ifndef _HENGRAPHICS_SHADER_H_
#define _HENGRAPHICS_SHADER_H_

#include "vendor/glm/glm.hpp"
#include "vendor/glm/gtc/matrix_transform.hpp"

#include <string>

namespace hen::graphics
{
    class Shader
    {
    public:

        Shader(const char* vsPath, const char* fsPath);

        void Activate();
        void Run();

        void SetVal(const std::string& name, bool val) const;
        void SetVal(const std::string& name, int val) const;
        void SetVal(const std::string& name, float val) const;

        void SetVec2(const std::string &name, const glm::vec2 &value) const;
        void SetVec3(const std::string &name, const glm::vec3 &value) const;
        void SetVec4(const std::string &name, const glm::vec4 &value) const;

        void SetMat2(const std::string &name, const glm::mat2 &mat) const;
        void SetMat3(const std::string &name, const glm::mat3 &mat) const;
        void SetMat4(const std::string &name, const glm::mat4 &mat) const;
    public:

        unsigned int ID;
    private:

        const char* m_VSPath;
        const char* m_FSPath;
    
        void CheckForCompileErrors(unsigned int shader, std::string type);
    };
}

#endif // !_HENGRAPHICS_SHADER_H_