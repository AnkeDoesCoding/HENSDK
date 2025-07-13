#include "graphics/henShader.h"

#include "vendor/glad/include/glad/glad.h"

#include "tools/henConsole.h"

#include <fstream>
#include <sstream>

namespace hen::graphics
{
    Shader::Shader(const char* vsPath, const char* fsPath)
    {
        m_VSPath = vsPath;
        m_FSPath = fsPath;
    }

    void Shader::Activate()
    {
        std::string vsSource, fsSource;
        std::ifstream vsFile, fsFile;

        unsigned int vertexShader, fragShader;
        int success;
        char infoLog[512];

        vsFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        fsFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

        try
        {
            vsFile.open(m_VSPath);
            fsFile.open(m_FSPath);

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
            console::Post("[hen::Shader] SHADER SOURCE FILE NOT SUCCESFULLY READ: " , console::Level::Error);
        }

        const char* vsCode = vsSource.c_str();
        const char* fsCode = fsSource.c_str();
        
        vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &vsCode, nullptr);
        glCompileShader(vertexShader);
        CheckForCompileErrors(vertexShader, "VERTEX");
        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

        if (!success)
        {
            glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
            console::Post("[hen::Shader] VERTEX SHADER COMPILATION FAILED: " + std::string(infoLog), console::Level::Error);
        }

        fragShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragShader, 1, &fsCode, nullptr);
        glCompileShader(fragShader);
        CheckForCompileErrors(fragShader, "FRAGMENT");
        glGetShaderiv(fragShader, GL_COMPILE_STATUS, &success);

        if (!success)
        {
            glGetShaderInfoLog(fragShader, 512, nullptr, infoLog);
            console::Post("[hen::Shader] FRAGMENT SHADER COMPILATION FAILED: " + std::string(infoLog), console::Level::Error);
        }

        ID = glCreateProgram();
        glAttachShader(ID, vertexShader);
        glAttachShader(ID, fragShader);
        glLinkProgram(ID);
        glGetProgramiv(ID, GL_LINK_STATUS, &success);

        if(!success)
        {
            glGetProgramInfoLog(ID, 512, nullptr, infoLog);
            console::Post("[hen::Shader] SHADER LINKING FAILED: \n" + std::string(infoLog), console::Level::Error);
        }

        glDeleteShader(vertexShader);
        glDeleteShader(fragShader);
    }

    void Shader::Run()
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

    void Shader::CheckForCompileErrors(unsigned int shader, std::string type)
    {
        int success;
        char infoLog[1024];
        if (type != "PROGRAM")
        {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success)
            {
                glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                console::Post("[hen::Shader] SHADER COMPILATION OF TYPE (" + type + ") FAILED: \n" + infoLog, console::Level::Error);
            }
        }
        else
        {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success)
            {
                glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                console::Post("[hen::Shader] SHADER COMPILATION OF TYPE (" + type + ") FAILED: \n" + infoLog, console::Level::Error);
            }
        }
    }
}

