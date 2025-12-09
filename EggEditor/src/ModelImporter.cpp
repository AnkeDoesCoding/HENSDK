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
            out.push_back(start + (uint32_t)src[i]);
        }
    }

    void LoadTextureFromIndex(tinygltf::Model& model, std::filesystem::path& modelDir, int index, hen::renderer::TextureHandle& handle)
    {
        if (index < 0 || index >= (int)model.textures.size())
        {
            return;
        }
                    
        int src = model.textures[index].source;
        if (src < 0 || src >= (int)model.images.size())
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

    void ImportModel(std::string path, hen::level::MeshComponent& meshComp)
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
            ok = loader.LoadASCIIFromFile(&model, &err, &warn, path), true;
        }

        if (!warn.empty())
        {
            hen::console::Log("[gltf] " + warn, hen::console::LOGLEVEL::WARNING);
        }

        if (!ok)
        {
            hen::console::Log("[gltf] " + err, hen::console::LOGLEVEL::ERROR);
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
                    hen::console::Log("[gltf] primitive missing POSITION", hen::console::LOGLEVEL::ERROR);
                    continue;
                }

                const tinygltf::Accessor& accPos = model.accessors[prim.attributes.at("POSITION")];
                if (!ReadAccessorAsFloats(pos, model, accPos))
                {
                    hen::console::Log("[gltf] failed to read POSITION accessor", hen::console::LOGLEVEL::ERROR);
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
                        hen::console::Log("[gltf] primitive has empty index accessor", hen::console::LOGLEVEL::WARNING);
                        subMesh.IndexCount = 0;
                    }
                    else
                    {
                        const tinygltf::BufferView& viewIdx = model.bufferViews[accIdx.bufferView];
                        const tinygltf::Buffer& bufIdx = model.buffers[viewIdx.buffer];
                        const unsigned char* indexData = bufIdx.data.data() + viewIdx.byteOffset + accIdx.byteOffset;

                        for (size_t i = 0; i < accIdx.count; ++i)
                        {
                            uint32_t idx = 0;
                            switch (accIdx.componentType)
                            {
                                case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
                                {
                                    const uint16_t* arr = reinterpret_cast<const uint16_t*>(indexData + i * tinygltf::GetComponentSizeInBytes(accIdx.componentType));
                                    idx = static_cast<uint32_t>(arr[0]);
                                    break;
                                }
                                case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
                                {
                                    const uint32_t* arr = reinterpret_cast<const uint32_t*>(indexData + i * tinygltf::GetComponentSizeInBytes(accIdx.componentType));
                                    idx = arr[0];
                                    break;
                                }
                                case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
                                {
                                    const uint8_t* arr = reinterpret_cast<const uint8_t*>(indexData + i * tinygltf::GetComponentSizeInBytes(accIdx.componentType));
                                    idx = static_cast<uint32_t>(arr[0]);
                                    break;
                                }
                                default:
                                    hen::console::Log("[gltf] unsupported index component type", hen::console::LOGLEVEL::ERROR);
                                    break;
                            }
                            meshComp.Indices.push_back(vertexStart + idx);
                        }

                        subMesh.IndexCount = static_cast<uint32_t>(accIdx.count);
                    }
                }

                // materials

                if (prim.material >= 0 && prim.material < (int)model.materials.size())
                {
                    const auto& material = model.materials[prim.material];
                    hen::level::MaterialComponent materialComp;

                    LoadTextureFromIndex(model, modelDir, material.pbrMetallicRoughness.baseColorTexture.index, materialComp.DiffuseTexture);

                    subMesh.Material = materialComp;
                }

                meshComp.SubMeshes.push_back(subMesh);
            }
        }

        meshComp.CreateRenderData();

        hen::console::Log("[importer] mesh import took " + std::to_string((int)std::round(timer.ElapsedMilliseconds())) + " ms");
    }
    
}