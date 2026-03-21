#include "henGraphics_OpenGL.h"

#include "tools/henConsole.h"

#include <glad/include/glad.h>

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
            default:
                HEN_ASSERT(false, "Couldn't find corresponding OpenGL type");
                return 0;
        }
    }

    Texture_OpenGL::~Texture_OpenGL()
    {
        glDeleteBuffers(1, &m_ID);
    }

    uint32_t Texture_OpenGL::GetID() const
    {
        return m_ID;
    }

    void Texture_OpenGL::CreateRenderData(int width, int height, int components, unsigned char* data)
    {
        GLenum internalFormat = 0;
        GLenum dataFormat = 0;

        if (components == 1)
        {
            internalFormat = GL_R8;
            dataFormat = GL_RED;
        }
        else if (components == 3)
        {
            internalFormat = GL_SRGB8_ALPHA8;
            dataFormat = GL_RGB;
        }
        else if (components == 4)  
        {
            internalFormat = GL_SRGB8_ALPHA8;
            dataFormat = GL_RGBA;
        }


        glCreateTextures(GL_TEXTURE_2D, 1, &m_ID);

        glTextureParameteri(m_ID, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTextureParameteri(m_ID, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTextureParameteri(m_ID, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTextureParameteri(m_ID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        int levels = 1 + static_cast<int>(std::floor(std::log2(std::max(width, height))));

        glTextureStorage2D(m_ID, levels, internalFormat, width, height);
        glTextureSubImage2D(m_ID, 0, 0, 0, width, height, dataFormat, GL_UNSIGNED_BYTE, data);
        glGenerateTextureMipmap(m_ID);
    }

    void Texture_OpenGL::CreateRenderData(int width, int height, int components, std::vector<unsigned char*> data)
    {
        GLenum internalFormat = 0;
        GLenum dataFormat = 0;

        if (components == 1)
        {
            internalFormat = GL_R8;
            dataFormat = GL_RED;
        }
        else if (components == 3)
        {
            internalFormat = GL_SRGB8_ALPHA8;
            dataFormat = GL_RGB;
        }
        else if (components == 4)  
        {
            internalFormat = GL_SRGB8_ALPHA8;
            dataFormat = GL_RGBA;
        }

        glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &m_ID);

        glTextureParameteri(m_ID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTextureParameteri(m_ID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTextureParameteri(m_ID, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        glTextureStorage2D(m_ID, 1, internalFormat, width, height);

        for (int i = 0; i < 6; i++)
        {
            glTextureSubImage3D(m_ID, 0, 0, 0, i, width, height, 1, dataFormat, GL_UNSIGNED_BYTE, data[i]);
        }
    }

    VertexBuffer_OpenGL::VertexBuffer_OpenGL(uint32_t size, float* vertices)
    {
        glCreateBuffers(1, &m_ID);
        glNamedBufferData(m_ID, size, vertices, GL_STATIC_DRAW);
    }

    VertexBuffer_OpenGL::~VertexBuffer_OpenGL()
    {
        glDeleteBuffers(1, &m_ID);
    }

    uint32_t VertexBuffer_OpenGL::GetID() const
    {
        return m_ID;
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
        glNamedBufferData(m_ID, count * sizeof(uint32_t), indices, GL_STATIC_DRAW);
    }

    IndexBuffer_OpenGL::~IndexBuffer_OpenGL()
    {
        glDeleteBuffers(1, &m_ID);
    }

    uint32_t IndexBuffer_OpenGL::GetCount() const
    {
        return m_Count;
    }

    uint32_t IndexBuffer_OpenGL::GetID() const
    {
        return m_ID;
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
        HEN_ASSERT(vertexBuffer->GetLayout().GetElements().size(), "Vertex buffer has no layout");

        const graphics::BufferLayout& layout = vertexBuffer->GetLayout();
        
        glVertexArrayVertexBuffer(m_ID, 0, vertexBuffer->GetID(), 0, layout.GetStride());

        for (const graphics::BufferElement& element : layout)
        {
            glEnableVertexArrayAttrib(m_ID, m_VertexBufferIndex);
            glVertexArrayAttribFormat(m_ID, m_VertexBufferIndex, element.GetComponentCount(), ShaderPrimitiveToOpenGLType(element.Type), element.Normalised ? GL_TRUE : GL_FALSE, static_cast<GLuint>(element.Offset));
            glVertexArrayAttribBinding(m_ID, m_VertexBufferIndex, 0);

            switch (element.Type)
            {
                case SHADER_PRIMITIVES::MAT3:
                case SHADER_PRIMITIVES::MAT4:
                {
                    uint8_t count = element.GetComponentCount();
                    for (uint8_t i = 1; i < count; i++)
                    {
                        m_VertexBufferIndex++;
                        glEnableVertexArrayAttrib(m_ID, m_VertexBufferIndex);
                        glVertexArrayAttribFormat(m_ID, m_VertexBufferIndex, 4, GL_FLOAT, GL_FALSE, static_cast<GLuint>(element.Offset + (i * sizeof(float) * 4)));
                        glVertexArrayAttribBinding(m_ID, m_VertexBufferIndex, 0);
                        glVertexArrayBindingDivisor(m_ID, 0, 1);
                    }
                    break;
                }
                default:
                    break;
            }

            m_VertexBufferIndex++;
        }

        m_VertexBuffers.push_back(vertexBuffer);
    }

    void VertexArray_OpenGL::SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer)
    {
        HEN_ASSERT(indexBuffer != nullptr, "Index buffer is nullptr");
        glVertexArrayElementBuffer(m_ID, indexBuffer->GetID());
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

    UniformBuffer_OpenGL::UniformBuffer_OpenGL(size_t size, unsigned binding)
    {
        m_Size = size;
        m_Binding = binding;
        glCreateBuffers(1, &m_ID);
        glNamedBufferData(m_ID, size, nullptr, GL_DYNAMIC_DRAW);
        glBindBufferBase(GL_UNIFORM_BUFFER, binding, m_ID);
    }

    void UniformBuffer_OpenGL::SetData(const void* data, size_t size, size_t offset)
    {
        m_Size = size;
        glNamedBufferSubData(m_ID, offset, size, data);
    }

    size_t UniformBuffer_OpenGL::GetSize() const
    {
        return m_Size;
    }

    unsigned UniformBuffer_OpenGL::GetBinding()
    {
        return m_Binding;
    }

    Shader_OpenGL::Shader_OpenGL(const char* vsPath, const char* fsPath)
        : m_VSPath(vsPath), m_FSPath(fsPath)
    {
        Compile();
    }

    void Shader_OpenGL::Compile()
    {
        std::string vsSource, fsSource;
        std::ifstream vsFile, fsFile;

        uint32_t vertexShader, fragShader;
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
        catch (std::ifstream::failure& e)
        {
            HEN_ERROR("[hen::Shader] Shader source file not successfully read: " + std::string(infoLog));
        }

        const char* vsCode = vsSource.c_str();
        const char* fsCode = fsSource.c_str();
        
        vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &vsCode, nullptr);
        glCompileShader(vertexShader);
        CheckForCompileErrors(vertexShader, SHADER_STAGES::VERTEX);

        fragShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragShader, 1, &fsCode, nullptr);
        glCompileShader(fragShader);
        CheckForCompileErrors(fragShader, SHADER_STAGES::FRAGMENT);

        m_ID = glCreateProgram();
        glAttachShader(m_ID, vertexShader);
        glAttachShader(m_ID, fragShader);
        glLinkProgram(m_ID);
        CheckForCompileErrors(m_ID, SHADER_STAGES::PROGRAM);

        glDeleteShader(vertexShader);
        glDeleteShader(fragShader);
    }

    void Shader_OpenGL::Bind()
    {
        glUseProgram(m_ID);
    }

    void Shader_OpenGL::UnBind()
    {
        glUseProgram(0);
    }

    uint32_t Shader_OpenGL::GetID() const
    {
        return m_ID;
    }

    void Shader_OpenGL::SetVal(const std::string& name, bool val) const
    {
        glUniform1i(glGetUniformLocation(m_ID, name.c_str()), static_cast<int>(val)); 
    }  

    void Shader_OpenGL::SetVal(const std::string& name, int val) const
    {
        glUniform1i(glGetUniformLocation(m_ID, name.c_str()), val);
    } 
       
    void Shader_OpenGL::SetVal(const std::string& name, float val) const
    {
        glUniform1f(glGetUniformLocation(m_ID, name.c_str()), val);
    }    

    void Shader_OpenGL::SetVec2(const std::string& name, const math::Vec2& value) const
    { 
        glUniform2fv(glGetUniformLocation(m_ID, name.c_str()), 1, &value[0]); 
    }

    void Shader_OpenGL::SetVec3(const std::string& name, const math::Vec3& value) const
    { 
        glUniform3fv(glGetUniformLocation(m_ID, name.c_str()), 1, &value[0]); 
    }
    
    void Shader_OpenGL::SetVec4(const std::string& name, const math::Vec4& value) const
    { 
        glUniform4fv(glGetUniformLocation(m_ID, name.c_str()), 1, &value[0]); 
    }

    void Shader_OpenGL::SetMat2(const std::string& name, const math::Matrix2& mat) const
    {
        glUniformMatrix2fv(glGetUniformLocation(m_ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
   
    void Shader_OpenGL::SetMat3(const std::string& name, const math::Matrix3& mat) const
    {
        glUniformMatrix3fv(glGetUniformLocation(m_ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

    void Shader_OpenGL::SetMat4(const std::string& name, const math::Matrix4& mat) const
    {
        glUniformMatrix4fv(glGetUniformLocation(m_ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

    void Shader_OpenGL::CheckForCompileErrors(uint32_t shader, SHADER_STAGES type)
    {
        int success;
        char infoLog[1024];
        
        switch (type)
        {
            case SHADER_STAGES::FRAGMENT:
                glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
                if (!success)
                {
                    glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
                    std::string msg = "[hen::Shader] Fragment shader compilation failed: \n";
                    msg += infoLog;
                    HEN_ERROR(msg);
                }
                break;
            case SHADER_STAGES::VERTEX:
                glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
                if (!success)
                {
                    glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
                    std::string msg = "[hen::Shader] Vertex shader compilation failed: \n";
                    msg += infoLog;
                    HEN_ERROR(msg);
                }
                break;
            case SHADER_STAGES::PROGRAM:
                glGetProgramiv(shader, GL_LINK_STATUS, &success);
                if (!success)
                {
                    glGetProgramInfoLog(shader, 1024, nullptr, infoLog);
                    std::string msg = "[hen::Shader] Shader program compilation failed: \n";
                    msg += infoLog;
                    HEN_ERROR(msg);
                }
                break;
            default:
                break;
        }
    }
}