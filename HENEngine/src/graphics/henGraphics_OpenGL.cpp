#include "henGraphics_OpenGL.h"

#include "vendor/glad/include/glad.h"

#include "tools/henConsole.h"

#include <fstream>
#include <sstream>


namespace hen::graphics
{
    static GLenum ShaderPrimitiveToOpenGLType(SHADER_PRIMITIVES type)
	{
		switch (type)
		{
			case SHADER_PRIMITIVES::FLOAT:    
                return GL_FLOAT;
			case SHADER_PRIMITIVES::FLOAT2:   
                return GL_FLOAT;
			case SHADER_PRIMITIVES::FLOAT3:   
                return GL_FLOAT;
			case SHADER_PRIMITIVES::FLOAT4:   
                return GL_FLOAT;
			case SHADER_PRIMITIVES::MAT3:     
                return GL_FLOAT;
			case SHADER_PRIMITIVES::MAT4:     
                return GL_FLOAT;
			case SHADER_PRIMITIVES::INT:      
                return GL_INT;
			case SHADER_PRIMITIVES::INT2:     
                return GL_INT;
			case SHADER_PRIMITIVES::INT3:     
                return GL_INT;
			case SHADER_PRIMITIVES::INT4:     
                return GL_INT;
			case SHADER_PRIMITIVES::BOOL:     
                return GL_BOOL;
		}

        console::Log("[hen::graphics] Couldn't find corresponding OpenGL type", console::LOGLEVEL::ERROR);
		return 0;
	}

    VertexBuffer_OpenGL::VertexBuffer_OpenGL(uint32_t size, float* vertices)
    {
        glCreateBuffers(1, &m_ID);
        glBindBuffer(GL_ARRAY_BUFFER, m_ID);
        glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
    }

    VertexBuffer_OpenGL::~VertexBuffer_OpenGL()
    {
        glDeleteBuffers(1, &m_ID);
    }

    void VertexBuffer_OpenGL::Bind() const
    {
        glBindBuffer(GL_ARRAY_BUFFER, m_ID);
    }

    void VertexBuffer_OpenGL::UnBind() const
    {  
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }   

    const BufferLayout& VertexBuffer_OpenGL::GetLayout() const
    {
        return m_Layout;
    }

    void VertexBuffer_OpenGL::SetLayout(const BufferLayout& layout)
    {
        m_Layout = layout;
    }

    IndexBuffer_OpenGL::IndexBuffer_OpenGL(uint32_t count, uint32_t* indices)
    {
        m_Count = count;
        glCreateBuffers(1, &m_ID);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ID);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t), indices, GL_STATIC_DRAW);
    }

    IndexBuffer_OpenGL::~IndexBuffer_OpenGL()
    {
        glDeleteBuffers(1, &m_ID);
    }

    uint32_t IndexBuffer_OpenGL::GetCount() const
    {
        return m_Count;
    }

    void IndexBuffer_OpenGL::Bind() const
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ID);
    }

    void IndexBuffer_OpenGL::UnBind() const
    {  
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }   

    VertexArray_OpenGL::VertexArray_OpenGL()
    {
        glCreateVertexArrays(1, &m_ID);
    }

    VertexArray_OpenGL::~VertexArray_OpenGL()
    {   
        glDeleteVertexArrays(1, &m_ID);
    }

    void VertexArray_OpenGL::Bind() const
    {
        glBindVertexArray(m_ID);
    }

    void VertexArray_OpenGL::UnBind() const
    {
        glBindVertexArray(0);
    }

    void VertexArray_OpenGL::AddVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer)
    {
        Bind();
        HEN_ASSERT(vertexBuffer != nullptr, "[hen::graphics] Vertex buffer is nullptr");
        vertexBuffer->Bind();

		const auto& layout = vertexBuffer->GetLayout();
		for (const auto& element : layout)
        {
            glEnableVertexAttribArray(m_VertexBufferIndex);

            if (element.IsIntegerType()) // ints, uints, bools
            {
                glVertexAttribIPointer(
                    m_VertexBufferIndex,
                    element.GetComponentCount(),
                    ShaderPrimitiveToOpenGLType(element.Type),
                    layout.GetStride(),
                    reinterpret_cast<const void*>(element.Offset)
                );
            }
            else // floats, vec2, vec3, vec4, etc.
            {
                glVertexAttribPointer(
                    m_VertexBufferIndex,
                    element.GetComponentCount(),
                    ShaderPrimitiveToOpenGLType(element.Type),
                    element.Normalised ? GL_TRUE : GL_FALSE,
                    layout.GetStride(),
                    reinterpret_cast<const void*>(element.Offset)
                );
            }
        }

		m_VertexBuffers.push_back(vertexBuffer);
    }

    void VertexArray_OpenGL::SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer)
    {
        Bind();
        HEN_ASSERT(indexBuffer != nullptr, "[hen::graphics] Index buffer is nullptr");
        indexBuffer->Bind();

        m_IndexBuffer = indexBuffer;
    }

    const std::vector<std::shared_ptr<VertexBuffer>>& VertexArray_OpenGL::GetVertexBuffers() const
    {
        return m_VertexBuffers;
    }

    const std::shared_ptr<IndexBuffer>& VertexArray_OpenGL::GetIndexBuffer() const
    {
        return m_IndexBuffer;
    }

    Shader_OpenGL::Shader_OpenGL(const char* vsPath, const char* fsPath)
        : m_VSPath(vsPath), m_FSPath(fsPath)
    {
        Activate();
    }

    void Shader_OpenGL::Activate()
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
            console::Log("[hen::Shader] SHADER SOURCE FILE NOT SUCCESFULLY READ: " + std::string(infoLog) , console::LOGLEVEL::ERROR);
        }

        const char* vsCode = vsSource.c_str();
        const char* fsCode = fsSource.c_str();
        
        vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &vsCode, nullptr);
        glCompileShader(vertexShader);
        CheckForCompileErrors(vertexShader, "VERTEX");

        fragShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragShader, 1, &fsCode, nullptr);
        glCompileShader(fragShader);
        CheckForCompileErrors(fragShader, "FRAGMENT");

        m_ID = glCreateProgram();
        glAttachShader(m_ID, vertexShader);
        glAttachShader(m_ID, fragShader);
        glLinkProgram(m_ID);
        CheckForCompileErrors(m_ID, "PROGRAM");

        glDeleteShader(vertexShader);
        glDeleteShader(fragShader);
    }

    void Shader_OpenGL::Run()
    {
        glUseProgram(m_ID);
    }

    void Shader_OpenGL::UnBind()
    {
        glUseProgram(0);
    }

    unsigned int Shader_OpenGL::GetID() const
    {
        return m_ID;
    }

    void Shader_OpenGL::SetVal(const std::string& name, bool val) const
    {
        glUniform1i(glGetUniformLocation(m_ID, name.c_str()), (int)val); 
    }  

    void Shader_OpenGL::SetVal(const std::string& name, int val) const
    {
        glUniform1i(glGetUniformLocation(m_ID, name.c_str()), val);
    } 
       
    void Shader_OpenGL::SetVal(const std::string& name, float val) const
    {
        glUniform1f(glGetUniformLocation(m_ID, name.c_str()), val); // finally fixed a bug that went unnoticed for a whole fucking month
    }    

    void Shader_OpenGL::SetVec2(const std::string &name, const glm::vec2 &value) const
    { 
        glUniform2fv(glGetUniformLocation(m_ID, name.c_str()), 1, &value[0]); 
    }

    void Shader_OpenGL::SetVec3(const std::string &name, const glm::vec3 &value) const
    { 
        glUniform3fv(glGetUniformLocation(m_ID, name.c_str()), 1, &value[0]); 
    }
    
    void Shader_OpenGL::SetVec4(const std::string &name, const glm::vec4 &value) const
    { 
        glUniform4fv(glGetUniformLocation(m_ID, name.c_str()), 1, &value[0]); 
    }

    void Shader_OpenGL::SetMat2(const std::string &name, const glm::mat2 &mat) const
    {
        glUniformMatrix2fv(glGetUniformLocation(m_ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
   
    void Shader_OpenGL::SetMat3(const std::string &name, const glm::mat3 &mat) const
    {
        glUniformMatrix3fv(glGetUniformLocation(m_ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

    void Shader_OpenGL::SetMat4(const std::string &name, const glm::mat4 &mat) const
    {
        glUniformMatrix4fv(glGetUniformLocation(m_ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

    void Shader_OpenGL::CheckForCompileErrors(unsigned int shader, std::string type)
    {
        int success;
        char infoLog[1024];
        if (type != "PROGRAM")
        {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success)
            {
                glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
                console::Log("[hen::Shader] " + type + " SHADER COMPILATION FAILED: \n" + infoLog, console::LOGLEVEL::ERROR);
            }
        }
        else
        {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success)
            {
                glGetProgramInfoLog(shader, 1024, nullptr, infoLog);
                console::Log("[hen::Shader] SHADER " + type + " COMPILATION FAILED: \n" + infoLog, console::LOGLEVEL::ERROR);
            }
        }
    }
}