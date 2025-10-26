#include "renderer/henRenderer_ResourceManagers.h"

// FINA-FUCKINGLY I GOT THIS SHIT WORKING
// FIRST I TRIED TO MAKE A GENERIC SLOTMAP TYPE SHIT WHICH WAS BUGGY AS FUCK BECAUSE I SUCK WITH TEMPLATES
// THEN I TRIED TO MAKE A SHADERMANAGER CLASS WITH THE SLOTMAP CLASS, IT FAILED OFCOURSE
// SO I SAID FUCK IT AND MERGED THE 2 TOGETHER

namespace hen::renderer
{
    size_t HashStrings(const std::string& a, const std::string& b)
    {
        size_t hashA = std::hash<std::string>{}(a);
        size_t hashB = std::hash<std::string>{}(b);
        return hashA ^ (hashB + 0x9e3779b97f4a7c15ULL + (hashA<<6) + (hashB>>2)); 
                    //                 ^
                    //                 |
                    //          what the fuck
    }

    size_t HashString(const std::string& a)
    {
        return std::hash<std::string>{}(a);
    }

    ShaderHandle ShaderManager::Load(const char* vsPath, const char* fsPath)
    {
        size_t key = HashStrings(vsPath, fsPath);

        auto it = m_PathToIndex.find(key);
        if (it != m_PathToIndex.end())
        {
            uint32_t index = it->second;
            return ShaderHandle{ index, m_Shaders[index].Generation };
        }

        uint32_t index;
        if (!m_FreeList.empty())
        {
            index = m_FreeList.back();
            m_FreeList.pop_back();

            m_Shaders[index].Shader.Destroy();                
            m_Shaders[index].Shader.Create(vsPath, fsPath);
            m_Shaders[index].Alive = true;
            m_Shaders[index].Generation++;
        }
        else
        {
            index = static_cast<uint32_t>(m_Shaders.size());
            m_Shaders.emplace_back();                         
            m_Shaders[index].Shader.Create(vsPath, fsPath);  
            m_Shaders[index].Alive = true;
            m_Shaders[index].Generation = 1;
        }

        m_PathToIndex[key] = index;

        return ShaderHandle{ index, m_Shaders[index].Generation };
    }

    graphics::Shader* ShaderManager::Get(const ShaderHandle& handle)
    {
        if (!handle.IsValid() || handle.Index >= m_Shaders.size() || !m_Shaders[handle.Index].Alive)
        {
            return nullptr;
        }

        return &m_Shaders[handle.Index].Shader;
    }

    void ShaderManager::Remove(const ShaderHandle& handle)
    {
        if (!handle.IsValid() || handle.Index >= m_Shaders.size() || !m_Shaders[handle.Index].Alive)
        {
            return;
        }

        m_ShaderSlot& slot = m_Shaders[handle.Index];

        if (!slot.Alive || slot.Generation != handle.Generation)
        {
            return;
        }

        slot.Shader.Destroy();
        slot.Alive = false;
        slot.Generation++;
        m_FreeList.push_back(handle.Index);

        for (auto it = m_PathToIndex.begin(); it != m_PathToIndex.end(); ++it)
        {
            if (it->second == handle.Index)
            {
                m_PathToIndex.erase(it);
                break;
            }
        }
    }

    TextureHandle TextureManager::Load(const char* texPath)
    {
        size_t key = HashString(texPath);

        auto it = m_PathToIndex.find(key);
        if (it != m_PathToIndex.end())
        {
            uint32_t index = it->second;
            return TextureHandle{ index, m_Textures[index].Generation };
        }

        uint32_t index;
        if (!m_FreeList.empty())
        {
            index = m_FreeList.back();
            m_FreeList.pop_back();

            m_Textures[index].Texture.Destroy();                
            m_Textures[index].Texture.Load(texPath); 
            m_Textures[index].Alive = true;
            m_Textures[index].Generation++;
        }
        else
        {
            index = static_cast<uint32_t>(m_Textures.size());
            m_Textures.emplace_back();                         
            m_Textures[index].Texture.Load(texPath);  
            m_Textures[index].Alive = true;
            m_Textures[index].Generation = 1;
        }

        m_PathToIndex[key] = index;

        return TextureHandle{ index, m_Textures[index].Generation };
    }

    graphics::Texture2D* TextureManager::Get(const TextureHandle& handle)
    {
        if (!handle.IsValid() || handle.Index >= m_Textures.size() || !m_Textures[handle.Index].Alive)
        {
            return nullptr;
        }

        return &m_Textures[handle.Index].Texture;
    }

    void TextureManager::Remove(const TextureHandle& handle)
    {
        if (!handle.IsValid() || handle.Index >= m_Textures.size() || !m_Textures[handle.Index].Alive)
        {
            return;
        }

        m_TextureSlot& slot = m_Textures[handle.Index];

        if (!slot.Alive || slot.Generation != handle.Generation)
        {
            return;
        }

        slot.Texture.Destroy();
        slot.Alive = false;
        slot.Generation++;
        m_FreeList.push_back(handle.Index);

        for (auto it = m_PathToIndex.begin(); it != m_PathToIndex.end(); ++it)
        {
            if (it->second == handle.Index)
            {
                m_PathToIndex.erase(it);
                break;
            }
        }
    }

}