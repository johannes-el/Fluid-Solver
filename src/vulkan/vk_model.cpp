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

	for (const auto& shape : shapes) {
		for (const auto& index : shape.mesh.indices) {
			Vertex vertex{};
			vertex.pos = {
				attrib.vertices[3 * index.vertex_index + 0],
				attrib.vertices[3 * index.vertex_index + 1],
				attrib.vertices[3 * index.vertex_index + 2]
			};

			if (index.texcoord_index >= 0) {
				vertex.texCoord = {
					attrib.texcoords[2 * index.texcoord_index + 0],
					1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
				};
			} else {
				vertex.texCoord = {0.0f, 0.0f};
			}

			vertex.color = {1.0f, 1.0f, 1.0f};

			context.vertices.push_back(vertex);
			context.indices.push_back(static_cast<uint32_t>(context.indices.size()));
		}
	}
}
