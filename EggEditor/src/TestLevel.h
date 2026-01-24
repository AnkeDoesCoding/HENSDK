#ifndef _TESTLEVEL_H_
#define _TESTLEVEL_H_

#include "HENEngine.h"

#include "ModelImporter.h"

// this motherfucker is jst a placeholder until i add proper level archiving and loading

namespace testlevel
{
    static hen::level::Level TestLevel;

    static hen::level::Entity* ModelEnt;
    static hen::level::Entity* LightEnt;

    void Load()
    {
        ModelEnt = new hen::level::Entity(hen::level::GetActiveLevel()->CreateEntity("model"));

        auto& transform = ModelEnt->AddComponent<hen::level::TransformComponent>();
        auto& mat = ModelEnt->AddComponent<hen::level::MaterialComponent>();
        auto& mesh = ModelEnt->AddComponent<hen::level::MeshComponent>();

        transform.SetScale(hen::math::Vec3(0.15f));

        hen::jobsystem::Execute([&mesh] {importer::ImportModel("res/models/sponza/sponza.glb", mesh);});

        
        mat.Shader = hen::renderer::GetShaderManager()->Load("res/engine/shaders/GLSL/BaseShaderVS.glsl", "res/engine/shaders/GLSL/BaseShaderFS.glsl");

        LightEnt = new hen::level::Entity(hen::level::GetActiveLevel()->CreateEntity("light"));

        auto& transformLight = LightEnt->AddComponent<hen::level::TransformComponent>();
        auto& light = LightEnt->AddComponent<hen::level::LightComponent>();

        transformLight.SetPosition(hen::math::Vec3(0.0f, 20.0f, -4.0f));

        light.Range = 500.0f;
        light.Intensity = 1.0f;
        light.Type = hen::level::LIGHT_TYPES::POINT;
    }

    void Delete()
    {
        delete ModelEnt;
        delete LightEnt;
    }
}

#endif // !_TESTLEVEL_H_