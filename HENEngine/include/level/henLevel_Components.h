#ifndef _HENLEVEL_COMPONENTS_H_
#define _HENLEVEL_COMPONENTS_H_

#include "core/henMath.h"
#include "graphics/henGraphics.h"
#include "renderer/henRenderer_ResourceManagers.h"

#include <string>

namespace hen::level
{
    enum class LIGHT_TYPES
    {
        POINT,
        SPOT,
        DIRECTIONAL
    };
    
    enum class COLLISIONSHAPES
    {
        BOX,
        SPHERE,
        CAPSULE,
        CYLINDER,
        CONVEX_HULL,
        TRIANGLE_MESH
    };

    struct NameComponent
    {
        std::string Name = "unknown";

        NameComponent() = default;
        
        NameComponent(const std::string& name)
            : Name(name)
        {

        }
    };

    struct TransformComponent
    {
        math::Vec3 LocalPosition = math::Vec3(0.0f);
        math::Quat LocalRotation = math::Quat(1.0f, 0.0f, 0.0f, 0.0f);
        math::Vec3 LocalScale = math::Vec3(1.0f);

        mutable math::Matrix4 World = math::Matrix4(1.0f);
        mutable bool Dirty = true;

        math::Matrix4 GetLocalMatrix() const
        {
            return math::Translate(math::Matrix4(1.0f), LocalPosition) * math::ToMatrix4(LocalRotation) * math::Scale(math::Matrix4(1.0f), LocalScale);
        }

        const math::Matrix4& GetWorldMatrix() const
        {
            if (Dirty)
            {
                math::Matrix4 local = GetLocalMatrix();
                World = local;
                Dirty = false;
            }

            return World;
        }

        math::Vec3 GetForwardVector() const
        {
            return math::Normalise(LocalRotation * math::Vec3(0.0f, 0.0f, 1.0f));
        }

        math::Vec3 GetRightVector() const
        {
            return math::Normalise(LocalRotation * math::Vec3(1.0f, 0.0f, 0.0f));
        }

        math::Vec3 GetUpVector() const
        {
            return math::Normalise(LocalRotation * math::Vec3(0.0f, 1.0f, 0.0f));
        }

        void SetLocalPosition(const math::Vec3& pos)
        {
            LocalPosition = pos;
            SetDirty();
        }

        void SetLocalRotation(const math::Quat& rot)
        {
            LocalRotation = rot;
            SetDirty();
        }

        void SetLocalScale(const math::Vec3& scale)
        {
            LocalScale = scale;
            SetDirty();
        }

        void SetEulerRotation(const math::Vec3& eulerDegrees)
        {
            LocalRotation = math::Quat(math::Radians(eulerDegrees));
            SetDirty();
        }

        math::Vec3 GetEulerRotation() const
        {
            return math::Degrees(math::EulerAngles(LocalRotation));
        }

        void SetDirty() const
        {
            Dirty = true;
        }

    };

    struct CameraComponent
    {
        float FOV = 90.0f;
        float NearPlane = 3.0f;
        float FarPlane = 1500.0f;

        math::Vec3 Position = math::Vec3(0.0f, 0.0f, 0.0f);
        math::Vec3 Rotation = math::Vec3(0.0f, 0.0f, 0.0f);
        math::Vec3 Front;
        math::Vec3 Right;
        math::Vec3 Up = math::Vec3(0.0f, 1.0f, 0.0f);

        CameraComponent() = default;

        CameraComponent(const float& fov, const math::Vec3& position, const math::Vec3& rotation)
            : FOV(fov), Position(position), Rotation(rotation)
        {

        } 

        math::Matrix4 GetViewMatrix()
        {
            return math::LookAt(Position, Position + Front, Up);
        }
        
        math::Matrix4 GetProjection(float x, float y)
        {
            return math::Perspective(math::Radians(FOV), x / y, NearPlane, FarPlane);
        }

        void SetDirty(const math::Vec3& levelUp)
        {
            math::Vec3 front;

            Rotation.x = math::Clamp(Rotation.x, -89.99f, 89.99f); // you can never truly look 90 up or down, hopefully this doesnt fuck up future calculations

            front.x = cos(math::Radians(Rotation.y)) * cos(math::Radians(Rotation.x));
            front.y = sin(math::Radians(Rotation.x));
            front.z = sin(math::Radians(Rotation.y)) * cos(math::Radians(Rotation.x));
            Front = math::Normalise(front);

            Right = math::Normalise(math::Cross(Front, levelUp));
            Up = math::Normalise(math::Cross(Right, Front));
        }

    };

    struct MaterialComponent
    {
        renderer::TextureHandle DiffuseTexture;
        renderer::TextureHandle SpecularTexture;
        renderer::ShaderHandle Shader;
        
        math::Vec3 Colour = math::Vec3(1.0f); // multiplied by textures if there any and acts as fallback if no textures found

        MaterialComponent() = default;
        MaterialComponent(const MaterialComponent& other) = default;
    };

    struct MeshComponent
    {
        std::vector<math::Vec3> Vertices;
        std::vector<math::Vec3> Normals;
        std::vector<uint32_t> Indices;
        std::vector<math::Vec2> TextureCoordinates;

        graphics::VertexArray VertexArray;
        std::shared_ptr<graphics::VertexBuffer> VertexBuffer;
        std::shared_ptr<graphics::IndexBuffer> IndexBuffer;

        graphics::BufferLayout BufferLayout
        {
            {graphics::SHADER_PRIMITIVES::FLOAT3, "aPos"},
            {graphics::SHADER_PRIMITIVES::FLOAT3, "aNormal"},
            {graphics::SHADER_PRIMITIVES::FLOAT2, "aTexCoord"}
        };

        struct SubMesh
        {
            uint32_t IndexStart = UINT32_MAX;
            uint32_t IndexCount = UINT32_MAX;
            uint32_t MaterialIndex = UINT32_MAX;
        };

        std::vector<SubMesh> SubMeshes;
        std::vector<MaterialComponent> Materials;

        graphics::RESOURCE_STATES State = graphics::RESOURCE_STATES::NOTREADY;

        MeshComponent() = default;

        void CreateRenderData()
        {
            HEN_ASSERT(Vertices.size() == Normals.size(), "Positions and Normals size mismatch");

            std::vector<float> interleavedBuffer;
            interleavedBuffer.reserve(Vertices.size() * 8);

            for (size_t i = 0; i < Vertices.size(); i++)
            {
                interleavedBuffer.push_back(Vertices[i].x);
                interleavedBuffer.push_back(Vertices[i].y);
                interleavedBuffer.push_back(Vertices[i].z);

                Normals[i] = math::Normalise(Normals[i]);

                interleavedBuffer.push_back(Normals[i].x);
                interleavedBuffer.push_back(Normals[i].y);
                interleavedBuffer.push_back(Normals[i].z);

                if (i < TextureCoordinates.size())
                {
                    interleavedBuffer.push_back(TextureCoordinates[i].x);
                    interleavedBuffer.push_back(TextureCoordinates[i].y);
                }
                else
                {
                    interleavedBuffer.push_back(0.0f);
                    interleavedBuffer.push_back(0.0f);
                }

            }
            
            VertexBuffer = graphics::VertexBuffer::Create(interleavedBuffer.size() * sizeof(float), interleavedBuffer.data());
            VertexBuffer->SetLayout(BufferLayout);

            IndexBuffer = graphics::IndexBuffer::Create(Indices.size(), Indices.data());

            VertexArray.Create();
            VertexArray.AddVertexBuffer(VertexBuffer);
            VertexArray.SetIndexBuffer(IndexBuffer);

            State = graphics::RESOURCE_STATES::READYTORENDER;
        }

        void DeleteRenderData()
        {
            if (VertexArray.IsBackendValid())
            {
                VertexArray.UnBind();
            }
        
            if (VertexBuffer)
            {
                VertexBuffer.reset();
            }
        
            if (IndexBuffer)
            {
                IndexBuffer.reset();
            }
            
            State = graphics::RESOURCE_STATES::NOTREADY;
        }

    };

    struct LightComponent
    {
        LIGHT_TYPES Type;

        float Range = 200.0f;
        float Intensity = 1.0f;
        float InnerCutOff = 25.0f;
        float OuterCutOff = 50.0f;

        math::Vec3 Colour = math::Vec3(1.0f, 1.0f, 1.0f);
        math::Vec3 Ambient = math::Vec3(0.05f, 0.05f, 0.05f);

        LightComponent() = default;

        LightComponent(LIGHT_TYPES type, float intensity)
            : Type(type), Intensity(intensity)
        {

        }
    };

    struct RigidBodyComponent
    {
        struct BoxParams
        {
            math::Vec3 HalfExtents = math::Vec3(1.0f);
        } Box;

        struct SphereParams
        {
            float Radius = 1.0f;
        } Sphere;

        struct CapsuleParams
        {
            float Radius = 1.0f;
            float Height = 1.0f;
        } Capsule;

        struct CylinderParams
        {
            float Radius = 1.0f;
            float Height = 1.0f;
        } Cylinder;

        COLLISIONSHAPES Shape = COLLISIONSHAPES::BOX;
        std::shared_ptr<void> PhysicsObject = nullptr;
        math::Vec3 Offset = math::Vec3(0.0f);

        bool Dirty = false;
        bool Kinematic = false;
        bool DisableDeactivation = false;
        bool StartDeactivated = false;

        float Mass = 1.0f;
        float Friction = 0.5f;
        float Restitution = 0.05f;
        float LinearDamping = 0.05f;
        float AngularDamping = 0.05f;
        float Bouyancy = 1.5f;

        void SetDirty()
        {
            Dirty = true;
        }
    };
}

#endif // !_HENLEVEL_COMPONENTS_H_