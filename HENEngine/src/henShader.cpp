#include "graphics/henShader.h"

#include "vendor/glad/include/glad/glad.h"

#include "tools/henConsole.h"

#include <fstream>
#include <sstream>
#include <iostream>

namespace hen::graphics
{
    Shader::Shader(const char* vsShaderPath, const char* fsShaderPath)
    {
        std::string vsSource, fsSource;
        std::ifstream vsFile, fsFile;

        unsigned int compiledVS, compiledFS;
        int success;
        char infoLog[512];

        vsFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        fsFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

        try
        {
            vsFile.open(vsShaderPath);
            fsFile.open(fsShaderPath);

            std::stringstream vsStream, fsStream;

            vsStream << vsFile.rdbuf();
            fsStream << fsFile.rdbuf();

            vsFile.close();
            fsFile.close();

            vsSource = vsStream.str();
            fsSource = fsStream.str();

        }
        catch(std::ifstream::failure& e)
        {
            console::Post("[hen::Shader] SHADER SOURCE FILE NOT SUCCESFULLY READ", console::Level::Error);
        }

        const char* vsCode = vsSource.c_str();
        const char* fsCode = fsSource.c_str();
        
        compiledVS = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(compiledVS, 1, &vsCode, nullptr);
        glCompileShader(compiledVS);
        checkCompileErrors(compiledVS, "VERTEX");
        glGetShaderiv(compiledVS, GL_COMPILE_STATUS, &success);

        if (!success)
        {
            glGetShaderInfoLog(compiledVS, 512, nullptr, infoLog);
            console::Post("[hen::Shader] VERTEX SHADER COMPILATION FAILED : " + std::string(infoLog), console::Level::Error);
        }

        compiledFS = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(compiledFS, 1, &fsCode, nullptr);
        glCompileShader(compiledFS);
        checkCompileErrors(compiledFS, "FRAGMENT");
        glGetShaderiv(compiledFS, GL_COMPILE_STATUS, &success);

        if (!success)
        {
            glGetShaderInfoLog(compiledFS, 512, nullptr, infoLog);
            console::Post("[hen::Shader] FRAGMENT SHADER COMPILATION FAILED : " + std::string(infoLog), console::Level::Error);
        }

        ID = glCreateProgram();
        glAttachShader(ID, compiledVS);
        glAttachShader(ID, compiledFS);
        glLinkProgram(ID);
        glGetProgramiv(ID, GL_LINK_STATUS, &success);

        if(!success)
        {
            glGetProgramInfoLog(ID, 512, nullptr, infoLog);
            console::Post("[hen::Shader] SHADER LINKING FAILED : " + std::string(infoLog), console::Level::Error);
        }

        glDeleteShader(compiledVS);
        glDeleteShader(compiledFS);
        
    }

    void Shader::Activate()
    {
        glUseProgram(ID);
    }

    void Shader::SetVal(const std::string& name, bool val) const
    {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)val); 
    }        
    void Shader::SetVal(const std::string& name, int val) const
    {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), val);
    }    
    void Shader::SetVal(const std::string& name, float val) const
    {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), val);
    }    

    void Shader::checkCompileErrors(unsigned int shader, std::string type)
    {
        int success;
        char infoLog[1024];
        if (type != "PROGRAM")
        {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success)
            {
                glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
        else
        {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success)
            {
                glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
    }
}

