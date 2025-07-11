#ifndef _HENSHADER_H_
#define _HENSHADER_H_


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
    public:

        unsigned int ID;
    private:

        const char* m_VSPath;
        const char* m_FSPath;
    
        void CheckForCompileErrors(unsigned int shader, std::string type);
    };
}

#endif // !_HENSHADER_H_