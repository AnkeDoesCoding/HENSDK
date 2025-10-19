#ifndef _HENRENDERER_SHADERMANAGER_
#define _HENRENDERER_SHADERMANAGER_

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

    class ShaderManager
    {
    public:

        ShaderHandle CreateOrGet(const char* vsPath, const char* fsPath);
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
}


#endif // !_HENRENDERER_SHADERMANAGER_