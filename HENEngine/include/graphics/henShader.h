#ifndef _HENSHADER_H_
#define _HENSHADER_H_


#include <string>

namespace hen::graphics
{
    class Shader
    {
    public:

        Shader(const char* vsShaderPath, const char* fsShaderPath);

        void Activate();

        void SetVal(const std::string& name, bool val) const;
        void SetVal(const std::string& name, int val) const;
        void SetVal(const std::string& name, float val) const;
    public:

        unsigned int ID;
    private:
    
        void checkCompileErrors(unsigned int shader, std::string type);
    };
}

#endif // !_HENSHADER_H_