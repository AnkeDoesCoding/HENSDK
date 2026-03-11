#include "ModelImporter.h"

// shit was a lot simpler with assimp, but shit took longer to compile with assimp. Thats a win icl
// squeezed the shit out of the importer and it is faster than assimp!!
// release: roughly 2.4% faster
// debug: over 59% faster holy shit
// yes i fucking benchmarked it (with sponza ofc)

#include "../vendor/tinygltf/tiny_gltf.h"

namespace importer
{
    bool ReadAccessorAsFloats(std::vector<float>& out, const tinygltf::Model& model, const tinygltf::Accessor& accessor)
    {
        if (accessor.bufferView < 0) 
        {
            return false;
        }

        const tinygltf::BufferView& view = model.bufferViews[accessor.bufferView];
        const tinygltf::Buffer& buffer = model.buffers[view.buffer];

        const size_t compCount = tinygltf::GetNumComponentsInType(accessor.type);
        const size_t count = accessor.count;

        out.clear();
        out.reserve(count * compCount);
        out.resize(count * compCount);

        const unsigned char* basePtr = buffer.data.data() + view.byteOffset + accessor.byteOffset;
        const size_t byteStride = (view.byteStride == 0) ? (compCount * tinygltf::GetComponentSizeInBytes(accessor.componentType)) : view.byteStride;

        for (size_t i = 0; i < count; ++i)
        {
            const unsigned char* elemPtr = basePtr + i * byteStride;

            for (size_t c = 0; c < compCount; ++c)
            {
                const unsigned char* compPtr = elemPtr + c * tinygltf::GetComponentSizeInBytes(accessor.componentType);

                float value = 0.0f;

                switch (accessor.componentType)
                {
                    case TINYGLTF_COMPONENT_TYPE_FLOAT:
                    {
                        float f = 0.0f;
                        memcpy(&f, compPtr, sizeof(float));
                        value = f;
                        break;
                    }
                    case TINYGLTF_COMPONENT_TYPE_SHORT:
                    {
                        int16_t v = 0;
                        memcpy(&v, compPtr, sizeof(int16_t));
                        if (accessor.normalized)
                        {
                            value = std::max(-1.0f, std::min(1.0f, v / 32767.0f));
                        }
                        else
                        {
                            value = static_cast<float>(v);
                        }
                        break;
                    }
                    case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
                    {
                        uint16_t v = 0;
                        memcpy(&v, compPtr, sizeof(uint16_t));
                        if (accessor.normalized)
                        {
                            value = std::max(0.0f, std::min(1.0f, v / 65535.0f));
                        }
                        else
                        {
                            value = static_cast<float>(v);
                        }
                        break;
                    }
                    case TINYGLTF_COMPONENT_TYPE_BYTE:
                    {
                        int8_t v = 0;
                        memcpy(&v, compPtr, sizeof(int8_t));
                        if (accessor.normalized)
                        {
                            value = std::max(-1.0f, std::min(1.0f, v / 127.0f));
                        }
                        else
                        {
                            value = static_cast<float>(v);
                        }
                        break;
                    }
                    case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
                    {
                        uint8_t v = 0;
                        memcpy(&v, compPtr, sizeof(uint8_t));
                        if (accessor.normalized)
                        {
                            value = std::max(0.0f, std::min(1.0f, v / 255.0f));
                        }
                        else
                        {
                            value = static_cast<float>(v);
                        }
                        break;
                    }
                    case TINYGLTF_COMPONENT_TYPE_INT:
                    case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
                    {
                        // this shit aint usually used but imma handle it for safety
                        if (tinygltf::GetComponentSizeInBytes(accessor.componentType) == 4)
                        {
                            if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_INT)
                            {
                                int32_t v = 0;
                                memcpy(&v, compPtr, sizeof(int32_t));
                                value = static_cast<float>(v);
                            }
                            else
                            {
                                uint32_t v = 0;
                                memcpy(&v, compPtr, sizeof(uint32_t));
                                value = static_cast<float>(v);
                            }
                        }
                        break;
                    }
                    default:
                        return false;
                        break;
                }

                out[i * compCount + c] = value;
            }
        }

        return true;
    }


    template<typename T>
    void CopyIndices(const unsigned char* data, size_t count, uint32_t start, std::vector<uint32_t>& out)
    {
        const T* src = reinterpret_cast<const T*>(data);
        for (size_t i = 0; i < count; i++)
        {
            out.push_back(start + static_cast<uint32_t>(src[i]));
        }
    }

    void LoadTextureFromIndex(tinygltf::Model& model, std::filesystem::path& modelDir, int index, hen::renderer::TextureHandle& handle)
    {
        if (index < 0 || index >= static_cast<int>(model.textures.size()))
        {
            return;
        }
                    
        int src = model.textures[index].source;
        if (src < 0 || src >= static_cast<int>(model.images.size()))
        {
            return;
        }
                    
        const auto& img = model.images[src];
                    
        // external textures
        if (!img.uri.empty())
        {
            auto full = modelDir / img.uri;
            handle = hen::renderer::GetTextureManager()->Load(full.string().c_str());
            return;
        }
                    
        // embedded textures
        if (!img.image.empty())
        {
            handle = hen::renderer::GetTextureManager()->Load(img.image.data(), img.image.size(), img.width, img.height, img.component);
            return;
        }
    }

    void ImportModel(std::string path, hen::level::MeshComponent& meshComp, hen::level::MaterialComponent& materialComp)
    {
        hen::Timer timer;

        tinygltf::TinyGLTF loader;
        tinygltf::Model model;
        std::string err, warn;

        loader.SetStoreOriginalJSONForExtrasAndExtensions(false);
        loader.SetPreserveImageChannels(false);

        bool ok = false;
        if (path.size() >= 4 && path.substr(path.size() - 4) == ".glb")
        {
            ok = loader.LoadBinaryFromFile(&model, &err, &warn, path, true);
        }
        else
        {
            ok = loader.LoadASCIIFromFile(&model, &err, &warn, path, true);
        }

        if (!warn.empty())
        {
            HEN_WARN("[gltf] " + warn);
        }

        if (!ok)
        {
            HEN_ERROR("[gltf] " + err);
            return;
        }

        std::filesystem::path modelDir = std::filesystem::path(path).parent_path();

        uint32_t totalVertexCount = 0;
        size_t totalIndexCount = 0;

        for (const auto& gltfMesh : model.meshes)
        {
            for (const auto& prim : gltfMesh.primitives)
            {
                if (prim.mode != TINYGLTF_MODE_TRIANGLES) continue;
                if (prim.attributes.find("POSITION") == prim.attributes.end()) continue;

                const auto& accPos = model.accessors.at(prim.attributes.at("POSITION"));
                totalVertexCount += static_cast<uint32_t>(accPos.count);

                if (prim.indices < 0)
                    totalIndexCount += accPos.count;
                else
                    totalIndexCount += model.accessors[prim.indices].count;
            }
        }


        meshComp.Vertices.reserve(totalVertexCount);
        meshComp.Normals.reserve(totalVertexCount);
        meshComp.TextureCoordinates.reserve(totalVertexCount);
        meshComp.Indices.reserve(totalIndexCount);

        for (const tinygltf::Mesh& gltfMesh : model.meshes)
        {
            for (const tinygltf::Primitive& prim : gltfMesh.primitives)
            {
                if (prim.mode != TINYGLTF_MODE_TRIANGLES)
                {
                    continue;
                }

                hen::level::MeshComponent::SubMesh subMesh;
                subMesh.IndexStart = meshComp.Indices.size();

                // vertex positions

                std::vector<float> pos;
                bool hasPositions = prim.attributes.find("POSITION") != prim.attributes.end();

                if (!hasPositions)
                {
                    HEN_ERROR("[gltf] primitive missing POSITION");
                    continue;
                }

                const tinygltf::Accessor& accPos = model.accessors[prim.attributes.at("POSITION")];
                if (!ReadAccessorAsFloats(pos, model, accPos))
                {
                    HEN_ERROR("[gltf] failed to read POSITION accessor");
                    continue;
                }

                const uint32_t vertexStart = static_cast<uint32_t>(meshComp.Vertices.size());
                const uint32_t vertexCount = static_cast<uint32_t>(accPos.count);

                for (uint32_t i = 0; i < vertexCount; ++i)
                {
                    const float* p = &pos[i * 3];
                    meshComp.Vertices.emplace_back(p[0], p[1], p[2]);
                }

                // normals

                std::vector<float> normals;
                bool hasNormals = (prim.attributes.find("NORMAL") != prim.attributes.end());

                if (hasNormals)
                {
                    const tinygltf::Accessor& accNorm = model.accessors[prim.attributes.at("NORMAL")];
                    if (!ReadAccessorAsFloats(normals, model, accNorm) || (accNorm.count != accPos.count))
                    {
                        hasNormals = false;
                    }
                }

                if (hasNormals)
                {
                    
                    for (uint32_t i = 0; i < vertexCount; i++)
                    {
                        const float* n = &normals[i * 3];
                        meshComp.Normals.emplace_back(n[0], n[1], n[2]);
                    }
                }
                else
                {
                    for (uint32_t i = 0; i < vertexCount; i++)
                    {
                        meshComp.Normals.emplace_back(0.0f, 1.0f, 0.0f);
                    }
                }

                // texture coordinates

                std::vector<float> texCoords;
                bool hasTexCoords = (prim.attributes.find("TEXCOORD_0") != prim.attributes.end());

                if (hasTexCoords)
                {
                    const tinygltf::Accessor& accUV = model.accessors[prim.attributes.at("TEXCOORD_0")];
                    if (!ReadAccessorAsFloats(texCoords, model, accUV) || (accUV.count != accPos.count))
                    {
                        hasTexCoords = false;
                    }
                }

                if (hasTexCoords)
                {
                    for (uint32_t i = 0; i < vertexCount; ++i)
                    {
                        const float* uv = &texCoords[i * 2];
                        meshComp.TextureCoordinates.emplace_back(uv[0], uv[1]);
                    }
                }
                else
                {
                    for (uint32_t i = 0; i < vertexCount; ++i)
                    {
                        meshComp.TextureCoordinates.emplace_back(0.0f, 0.0f);
                    }
                }

                // indices

                if (prim.indices < 0)
                {
                    for (uint32_t i = 0; i < vertexCount; ++i)
                    {
                        meshComp.Indices.push_back(vertexStart + i);
                    }   

                    subMesh.IndexCount = vertexCount;
                }
                else
                {
                    const tinygltf::Accessor& accIdx = model.accessors[prim.indices];
                    if (accIdx.count == 0)
                    {
                        HEN_WARN("[gltf] primitive has empty index accessor");
                        subMesh.IndexCount = 0;
                    }
                    else
                    {
                        const tinygltf::BufferView& viewIdx = model.bufferViews[accIdx.bufferView];
                        const tinygltf::Buffer& bufIdx = model.buffers[viewIdx.buffer];
                                            
                        const unsigned char* basePtr = bufIdx.data.data() + viewIdx.byteOffset + accIdx.byteOffset;
                                            
                        size_t stride = (viewIdx.byteStride != 0) ? viewIdx.byteStride : tinygltf::GetComponentSizeInBytes(accIdx.componentType);
                                            
                        for (size_t i = 0; i < accIdx.count; ++i)
                        {
                            const unsigned char* ptr = basePtr + i * stride;
                        
                            uint32_t idx = 0;
                            switch (accIdx.componentType)
                            {
                                case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
                                    idx = *reinterpret_cast<const uint8_t*>(ptr);
                                    break;
                                case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
                                    idx = *reinterpret_cast<const uint16_t*>(ptr);
                                    break;
                                case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
                                    idx = *reinterpret_cast<const uint32_t*>(ptr);
                                    break;
                                default:
                                    HEN_ERROR("[gltf] unsupported index component type");
                                    break;
                            }
                        
                            meshComp.Indices.push_back(vertexStart + idx);
                        }

                        subMesh.IndexCount = static_cast<uint32_t>(accIdx.count);
                    }
                }

                // materials

                if (prim.material >= 0 && prim.material < static_cast<int>(model.materials.size()))
                {
                    const auto& material = model.materials[prim.material];

                    hen::renderer::TextureHandle diffuseHandle;

                    LoadTextureFromIndex(model, modelDir, material.pbrMetallicRoughness.baseColorTexture.index, diffuseHandle);

                    materialComp.DiffuseTextures.push_back(diffuseHandle);
                    subMesh.DiffuseIndex = static_cast<uint32_t>(materialComp.DiffuseTextures.size() - 1);

                    hen::renderer::TextureHandle specularHandle;

                    LoadTextureFromIndex(model, modelDir, material.pbrMetallicRoughness.metallicRoughnessTexture.index, specularHandle);

                    materialComp.SpecularTextures.push_back(specularHandle);
                    subMesh.SpecularIndex = static_cast<uint32_t>(materialComp.SpecularTextures.size() - 1);

                }

                meshComp.SubMeshes.push_back(subMesh);
            }
        }
        
        meshComp.State = hen::graphics::RESOURCE_STATES::READY_TO_UPLOAD;

        HEN_LOG("[importer] mesh import took " + std::to_string(static_cast<int>(std::round(timer.ElapsedMilliseconds()))) + " ms");
    }
    
}