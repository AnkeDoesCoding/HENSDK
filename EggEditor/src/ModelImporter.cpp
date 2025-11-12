#include "ModelImporter.h"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

namespace importer
{
    void ProcessMesh(aiMesh* mesh, hen::level::MeshComponent& meshComp)
    {
        unsigned int vertexOffset = static_cast<unsigned int>(meshComp.Verticies.size());

        for (unsigned int i = 0; i < mesh->mNumVertices; i++)
        {
            const aiVector3D& pos = mesh->mVertices[i];
            const aiVector3D normal = mesh->HasNormals() ? mesh->mNormals[i] : aiVector3D(0.0f, 1.0f, 0.0f);

            meshComp.Verticies.emplace_back(pos.x, pos.y, pos.z);
            meshComp.Normals.emplace_back(normal.x, normal.y, normal.z);

            if (mesh->mTextureCoords[0])
            {
                const aiVector3D& uv = mesh->mTextureCoords[0][i];
                meshComp.TextureCoordinates.emplace_back(uv.x, uv.y);
            }
            else
            {
                meshComp.TextureCoordinates.emplace_back(0.0f, 0.0f);
            }
        }

        for (unsigned int i = 0; i < mesh->mNumFaces; i++)
        {
            const aiFace& face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++)
            {
                meshComp.Indicies.push_back(face.mIndices[j] + vertexOffset);
            }
        }
        
    }

    void ProcessNode(aiNode* node, const aiScene* scene, hen::level::MeshComponent& meshComp)
    
    {
        for (unsigned int i = 0; i < node->mNumMeshes; i++)
        {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            ProcessMesh(mesh, meshComp);
        }

        for (unsigned int i = 0; i < node->mNumChildren; i++)
        {
            ProcessNode(node->mChildren[i], scene, meshComp);
        }
    }

    void ImportModel(std::string path, hen::level::MeshComponent& meshComp)
    {
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(
            path,
            aiProcess_Triangulate |
            aiProcess_FlipUVs |
            aiProcess_GenSmoothNormals |
            aiProcess_JoinIdenticalVertices |
            aiProcess_PreTransformVertices |
            aiProcess_ImproveCacheLocality |
            aiProcess_OptimizeMeshes
        );

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            hen::console::Log(std::string("[assimp] ") + importer.GetErrorString(), hen::console::LOGLEVEL::ERROR);
            return;
        }
    
        ProcessNode(scene->mRootNode, scene, meshComp);

        meshComp.CreateRenderData();
    }
    
}