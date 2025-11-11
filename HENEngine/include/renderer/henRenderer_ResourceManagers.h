#ifndef _HENRENDERER_RESOURCEMANAGERS_
#define _HENRENDERER_RESOURCEMANAGERS_

#include "graphics/henGraphics.h"

namespace hen::renderer
{
    struct ShaderHandle
    {
        uint32_t Index = UINT32_MAX; // deadass just found out that this exists, looks better than using 0 imo
        uint32_t Generation = UINT32_MAX;

        bool IsValid() const 
        { 
            return Index != UINT32_MAX; 
        }
    };

    struct TextureHandle
    {
        uint32_t Index = UINT32_MAX;
        uint32_t Generation = UINT32_MAX;

        bool IsValid() const 
        { 
            return Index != UINT32_MAX; 
        }
    };

    class ShaderManager
    {
    public:

        ShaderHandle Load(const char* vsPath, const char* fsPath);
        graphics::Shader* Get(const ShaderHandle& handle);
        void Remove(const ShaderHandle& handle);
    
    private:

        struct m_ShaderSlot
        {
            graphics::Shader Shader;
            bool Alive = false;
            uint32_t Generation = 1;
        };

        std::vector<m_ShaderSlot> m_Shaders;
        std::unordered_map<size_t, uint32_t> m_PathToIndex;
        std::vector<uint32_t> m_FreeList;
    };

    class TextureManager
    {
    public:

        TextureHandle Load(const char* texPath);
        graphics::Texture* Get(const TextureHandle& handle);
        void Remove(const TextureHandle& handle); 

    private:

        struct m_TextureSlot
        {
            graphics::Texture Texture;
            bool Alive = false;
            uint32_t Generation = 1;
        };

        std::vector<m_TextureSlot> m_Textures;
        std::unordered_map<size_t, uint32_t> m_PathToIndex;
        std::vector<uint32_t> m_FreeList;
    };

    inline ShaderManager*& GetShaderManager()
    {
        static ShaderManager* shaderManager = nullptr;
		return shaderManager;
    }

    inline TextureManager*& GetTextureManager()
    {
        static TextureManager* textureManager = nullptr;
        return textureManager;
    }
}


#endif // !_HENRENDERER_RESOURCEMANAGERS_