#ifndef _TESTLEVEL_H_
#define _TESTLEVEL_H_

#include "HENEngine.h"

#include "ModelImporter.h"

// this motherfucker is jst a placeholder until i add proper level archiving and loading

namespace testlevel
{
    static hen::level::Level TestLevel;

    static hen::level::Entity ModelEnt;
    static hen::level::Entity LightEnt;
    static hen::level::Entity CubeEnt;
    static hen::level::Entity FloorEnt;

    void Load()
    {
        // Lights

        LightEnt = hen::level::GetActiveLevel()->CreateEntity("light");
        auto& lightTransform = LightEnt.AddComponent<hen::level::TransformComponent>();
        auto& light = LightEnt.AddComponent<hen::level::LightComponent>();

        lightTransform.SetLocalPosition(hen::math::Vec3(0.0f, 25.0f, 15.0f));
        light.Range = 2500.0f;

        // Sponza

        ModelEnt = hen::level::GetActiveLevel()->CreateEntity("model");
        auto& modelTransform = ModelEnt.AddComponent<hen::level::TransformComponent>();
        auto& modelMat = ModelEnt.AddComponent<hen::level::MaterialComponent>();
        auto& modelMesh = ModelEnt.AddComponent<hen::level::MeshComponent>();
        auto& modelRB = ModelEnt.AddComponent<hen::level::RigidBodyComponent>();
        
        hen::jobsystem::Execute([&modelMesh, &modelMat] {importer::ImportModel("res/models/sponza/sponza.glb", modelMesh, modelMat);});
        modelMat.Shader = hen::renderer::GetShaderManager()->Load("res/engine/shaders/GLSL/BaseShaderVS.glsl", "res/engine/shaders/GLSL/BaseShaderFS.glsl");

        modelRB.Kinematic = true;
        modelRB.Shape = hen::level::COLLISION_SHAPES::TRIANGLE_MESH;

        // Cube

        CubeEnt = hen::level::GetActiveLevel()->CreateEntity("cube");
        auto& cubeTransform = CubeEnt.AddComponent<hen::level::TransformComponent>();
        auto& cubeMesh = CubeEnt.AddComponent<hen::level::MeshComponent>();
        auto& cubeRB = CubeEnt.AddComponent<hen::level::RigidBodyComponent>();
        auto& cubeMat = CubeEnt.AddComponent<hen::level::MaterialComponent>();

        cubeRB.Mass = 10.0f;
        cubeRB.Restitution = 0.2f;

        cubeTransform.SetLocalPosition(hen::math::Vec3(20.0f, 100.0f, 0.0f));

        importer::ImportModel("res/models/primitives/cube.glb", cubeMesh, cubeMat);
        cubeMat.Shader = hen::renderer::GetShaderManager()->Load("res/engine/shaders/GLSL/BaseShaderVS.glsl", "res/engine/shaders/GLSL/BaseShaderFS.glsl");

        // Level skybox

        importer::ImportModel("res/models/skybox/skybox.glb", TestLevel.Skybox.Mesh, TestLevel.Skybox.Material);

        hen::graphics::TextureDesc skyboxTexDesc;

        skyboxTexDesc.Cubemap = true;
        skyboxTexDesc.PathToFaces = 
        {
            "res/engine/textures/cityskybox/right.jpg",
            "res/engine/textures/cityskybox/left.jpg",
            "res/engine/textures/cityskybox/top.jpg",
            "res/engine/textures/cityskybox/bottom.jpg",
            "res/engine/textures/cityskybox/front.jpg",
            "res/engine/textures/cityskybox/back.jpg"
        };

        TestLevel.Skybox.Cubemap = hen::renderer::GetTextureManager()->Load(skyboxTexDesc);
    }
}

#endif // !_TESTLEVEL_H_