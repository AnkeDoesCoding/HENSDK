#include "ModelImporter.h"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

namespace importer
{
    void ProcessMesh(aiMesh* mesh, const aiScene* scene, hen::level::MeshComponent& meshComp, hen::level::MaterialComponent& materialComp, std::string path)
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

        if (mesh->mMaterialIndex >= 0)
        {
            aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

            for (unsigned int i = 0; i < material->GetTextureCount(aiTextureType_DIFFUSE); i++)
            {
                aiString str;
                material->GetTexture(aiTextureType_DIFFUSE, i, &str);

                std::filesystem::path fullPath = path;
                fullPath /= str.C_Str(); // append the relative path from the .mtl

                std::string finalPath = fullPath.string();

                hen::console::Log(finalPath);

                materialComp.DiffuseTexture = hen::renderer::GetTextureManager()->Load(finalPath.c_str());
            }
        }
        
    }

    void ProcessNode(aiNode* node, const aiScene* scene, hen::level::MeshComponent& meshComp, hen::level::MaterialComponent& materialComp, std::string path)
    
    {
        for (unsigned int i = 0; i < node->mNumMeshes; i++)
        {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            ProcessMesh(mesh, scene, meshComp, materialComp, path);
        }

        for (unsigned int i = 0; i < node->mNumChildren; i++)
        {
            ProcessNode(node->mChildren[i], scene, meshComp, materialComp, path);
        }
    }

    void ImportModel(std::string path, hen::level::MeshComponent& meshComp, hen::level::MaterialComponent& materialComp)
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

        std::filesystem::path p = path;
    std::string modelDir = p.parent_path().string();
    
        ProcessNode(scene->mRootNode, scene, meshComp, materialComp, modelDir);

        meshComp.CreateRenderData();
    }
    
}