#ifndef _COMPONENTWINDOW_H_
#define _COMPONENTWINDOW_H_

#include "HENEngine.h"

class ComponentWindow
{
public:
    void Initialise();
    
public:
    hen::level::Entity SelectedEntity;

private:
    hen::renderer::TextureHandle m_ResetIcon;
    hen::renderer::TextureHandle m_RemoveIcon;
    hen::renderer::TextureHandle m_AddIcon;
    hen::renderer::TextureHandle m_TransformIcon;
    hen::renderer::TextureHandle m_MaterialIcon;
    hen::renderer::TextureHandle m_RigidBodyIcon;
    hen::renderer::TextureHandle m_MeshIcon;
    hen::renderer::TextureHandle m_LightIcon;
    hen::renderer::TextureHandle m_NameIcon;

};

#endif // !_COMPONENTWINDOW_H_