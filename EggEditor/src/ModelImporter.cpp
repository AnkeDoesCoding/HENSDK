#include "ModelImporter.h"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

namespace importer
{
    void ProcessMesh(aiMesh* mesh, const aiScene* scene, hen::level::MeshComponent& meshComp,  const aiMatrix4x4& transform)
    {
        unsigned int vertexOffset = static_cast<unsigned int>(meshComp.Verticies.size());

        aiMatrix3x3 normalMatrix = aiMatrix3x3(transform);
        normalMatrix.Inverse();
        normalMatrix.Transpose();

        for (unsigned int i = 0; i < mesh->mNumVertices; i++)
        {
            aiVector3D pos = mesh->mVertices[i];
            aiVector3D normal = mesh->HasNormals() ? mesh->mNormals[i] : aiVector3D(0.0f, 1.0f, 0.0f);

            aiVector3D transformedPos = transform * pos;
            aiVector3D transformedNormal = normalMatrix * normal;
            transformedNormal.Normalize();

            meshComp.Verticies.emplace_back(transformedPos.x * 0.01f, transformedPos.y * 0.01f, transformedPos.z * 0.01f);
            meshComp.Normals.emplace_back(transformedNormal.x, transformedNormal.y, transformedNormal.z);
        }

        for (unsigned int i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++)
            {
                meshComp.Indicies.push_back(face.mIndices[j] + vertexOffset);
            }
        }

    }

    void ProcessNode(aiNode* node, const aiScene* scene, hen::level::MeshComponent& meshComp, const aiMatrix4x4& parentTransform)
    {
        aiMatrix4x4 currentTransform = parentTransform * node->mTransformation;

        for (unsigned int i = 0; i < node->mNumMeshes; i++)
        {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            ProcessMesh(mesh, scene, meshComp, currentTransform);
        }

        for (unsigned int i = 0; i < node->mNumChildren; i++)
        {
            ProcessNode(node->mChildren[i], scene, meshComp, currentTransform);
        }
    }

    void ImportModel(std::string path, hen::level::MeshComponent& meshComp)
    {
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenSmoothNormals | aiProcess_JoinIdenticalVertices); 

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            hen::console::Log(std::string("[assimp] ") + importer.GetErrorString(), hen::console::LOGLEVEL::ERROR);
            return;
        }
    
        aiMatrix4x4 identity;
        ProcessNode(scene->mRootNode, scene, meshComp, identity);

        meshComp.CreateRenderData();
    }
    
}