#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"
#include "vulkan/vk_context.hpp"
#include "vulkan/vk_vertex.hpp"

#include "includes.hpp"
#include "vulkan/vk_model.hpp"

void loadModel(VkContext& context, const std::string model_path)
{
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, model_path.c_str())) {
        throw std::runtime_error(warn + err);
    }

    struct VertexKey {
        int pos_idx, tex_idx, norm_idx;
        bool operator<(const VertexKey& other) const {
            return std::tie(pos_idx, tex_idx, norm_idx) < std::tie(other.pos_idx, other.tex_idx, other.norm_idx);
        }
    };

    std::map<VertexKey, uint32_t> uniqueVertices;

    for (const auto& shape : shapes) {
        for (const auto& idx : shape.mesh.indices) {
            VertexKey key{ idx.vertex_index, idx.texcoord_index, idx.normal_index };

            if (uniqueVertices.count(key) == 0) {
                Vertex vertex{};
                vertex.pos = {
                    attrib.vertices[3 * idx.vertex_index + 0],
                    attrib.vertices[3 * idx.vertex_index + 1],
                    attrib.vertices[3 * idx.vertex_index + 2]
                };

                if (idx.texcoord_index >= 0) {
                    vertex.texCoord = {
                        attrib.texcoords[2 * idx.texcoord_index + 0],
                        1.0f - attrib.texcoords[2 * idx.texcoord_index + 1]
                    };
                } else {
                    vertex.texCoord = {0.0f, 0.0f};
                }

                vertex.color = {1.0f, 1.0f, 1.0f};

                uint32_t newIndex = static_cast<uint32_t>(context.vertices.size());
                context.vertices.push_back(vertex);
                uniqueVertices[key] = newIndex;
            }

            context.indices.push_back(uniqueVertices[key]);
        }
    }
}
