#include "ModelImporter.h"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

namespace importer
{
    void ProcessMesh(aiMesh* mesh, const aiScene* scene, hen::level::MeshComponent& meshComp, std::string path)
    {
        unsigned int vertexOffset = static_cast<unsigned int>(meshComp.Vertices.size());
        unsigned int indexOffset = static_cast<unsigned int>(meshComp.Indices.size());
        
        for (unsigned int i = 0; i < mesh->mNumVertices; i++)
        {
            const aiVector3D& pos = mesh->mVertices[i];
            const aiVector3D normal = mesh->HasNormals() ? mesh->mNormals[i] : aiVector3D(0.0f, 1.0f, 0.0f);

            meshComp.Vertices.emplace_back(pos.x, pos.y, pos.z);
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

        // Copy indices
        for (unsigned int i = 0; i < mesh->mNumFaces; i++)
        {
            const aiFace& face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++)
            {
                meshComp.Indices.push_back(face.mIndices[j] + vertexOffset);
            }
        }

        hen::level::MeshComponent::SubMesh submesh;
        submesh.IndexStart = indexOffset;
        submesh.IndexCount = mesh->mNumFaces * 3;

        if (mesh->mMaterialIndex >= 0)
        {
            aiMaterial* mat = scene->mMaterials[mesh->mMaterialIndex];
            hen::level::MaterialComponent material;
        
            if (mat->GetTextureCount(aiTextureType_DIFFUSE) > 0)
            {
                aiString str;
                mat->GetTexture(aiTextureType_DIFFUSE, 0, &str);
                std::filesystem::path fullPath = path;
                fullPath /= str.C_Str();
                material.DiffuseTexture = hen::renderer::GetTextureManager()->Load(fullPath.string().c_str());
            }
        
            // if (mat->GetTextureCount(aiTextureType_SPECULAR) > 0)
            // {
            //     aiString str;
            //     mat->GetTexture(aiTextureType_SPECULAR, 0, &str);
            //     std::filesystem::path fullPath = path;
            //     fullPath /= str.C_Str();
            //     material.SpecularTexture = hen::renderer::GetTextureManager()->Load(fullPath.string().c_str());
            // }
        
            submesh.Material = material;
        }

        meshComp.SubMeshes.push_back(submesh);
    }

    void ProcessNode(aiNode* node, const aiScene* scene, hen::level::MeshComponent& meshComp, std::string path)
    
    {
        for (unsigned int i = 0; i < node->mNumMeshes; i++)
        {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            ProcessMesh(mesh, scene, meshComp, path);
        }

        for (unsigned int i = 0; i < node->mNumChildren; i++)
        {
            ProcessNode(node->mChildren[i], scene, meshComp, path);
        }
    }

    void ImportModel(std::string path, hen::level::MeshComponent& meshComp)
    {
        hen::Timer timer;

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
    
        ProcessNode(scene->mRootNode, scene, meshComp, modelDir);

        meshComp.CreateRenderData();

        hen::console::Log("[importer] Mesh import took " + std::to_string((int)std::round(timer.ElapsedMilliseconds())) + " ms");
    }
    
}