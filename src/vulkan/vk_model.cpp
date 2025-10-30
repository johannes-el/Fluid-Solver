#define TINYGLTF_IMPLEMENTATION
#define TINYGLTF_USE_STB_IMAGE_WRITE
#include <tiny_gltf.h>

#include "vulkan/vk_context.hpp"
#include "vulkan/vk_vertex.hpp"

#include "includes.hpp"
#include "vulkan/vk_model.hpp"


void loadModel(VkContext& context, std::string model_path) {
	// Use tinygltf to load the model instead of tinyobjloader
	tinygltf::Model model;
	tinygltf::TinyGLTF loader;
	std::string err;
	std::string warn;

	bool ret = loader.LoadASCIIFromFile(&model, &err, &warn, model_path);

	if (!warn.empty()) {
		std::cout << "glTF warning: " << warn << std::endl;
	}

	if (!err.empty()) {
		std::cout << "glTF error: " << err << std::endl;
	}

	if (!ret) {
		throw std::runtime_error("Failed to load glTF model");
	}

	// Process all meshes in the model
	std::unordered_map<Vertex, uint32_t> uniqueVertices{};

	for (const auto& mesh : model.meshes) {
		for (const auto& primitive : mesh.primitives) {
			// Get indices
			const tinygltf::Accessor& indexAccessor = model.accessors[primitive.indices];
			const tinygltf::BufferView& indexBufferView = model.bufferViews[indexAccessor.bufferView];
			const tinygltf::Buffer& indexBuffer = model.buffers[indexBufferView.buffer];

			// Get vertex positions
			const tinygltf::Accessor& posAccessor = model.accessors[primitive.attributes.at("POSITION")];
			const tinygltf::BufferView& posBufferView = model.bufferViews[posAccessor.bufferView];
			const tinygltf::Buffer& posBuffer = model.buffers[posBufferView.buffer];

			// Get texture coordinates if available
			bool hasTexCoords = primitive.attributes.find("TEXCOORD_0") != primitive.attributes.end();
			const tinygltf::Accessor* texCoordAccessor = nullptr;
			const tinygltf::BufferView* texCoordBufferView = nullptr;
			const tinygltf::Buffer* texCoordBuffer = nullptr;

			if (hasTexCoords) {
				texCoordAccessor = &model.accessors[primitive.attributes.at("TEXCOORD_0")];
				texCoordBufferView = &model.bufferViews[texCoordAccessor->bufferView];
				texCoordBuffer = &model.buffers[texCoordBufferView->buffer];
			}

			// Process vertices
			for (size_t i = 0; i < posAccessor.count; i++) {
				Vertex vertex{};

				// Get position
				const float* pos = reinterpret_cast<const float*>(&posBuffer.data[posBufferView.byteOffset + posAccessor.byteOffset + i * 12]);
				vertex.pos = {pos[0], pos[1], pos[2]};

				// Get texture coordinates if available
				if (hasTexCoords) {
					const float* texCoord = reinterpret_cast<const float*>(&texCoordBuffer->data[texCoordBufferView->byteOffset + texCoordAccessor->byteOffset + i * 8]);
					vertex.texCoord = {texCoord[0], texCoord[1]};
				} else {
					vertex.texCoord = {0.0f, 0.0f};
				}

				// Set default color
				vertex.color = {1.0f, 1.0f, 1.0f};

				// Add vertex if unique
				if (!uniqueVertices.contains(vertex)) {
					uniqueVertices[vertex] = static_cast<uint32_t>(context.vertices.size());
					context.vertices.push_back(vertex);
				}
			}

			// Process indices
			const unsigned char* indexData = &indexBuffer.data[indexBufferView.byteOffset + indexAccessor.byteOffset];

			// Handle different index component types
			if (indexAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT) {
				const uint16_t* indices16 = reinterpret_cast<const uint16_t*>(indexData);
				for (size_t i = 0; i < indexAccessor.count; i++) {
					Vertex vertex = context.vertices[indices16[i]];
					context.indices.push_back(uniqueVertices[vertex]);
				}
			} else if (indexAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT) {
				const uint32_t* indices32 = reinterpret_cast<const uint32_t*>(indexData);
				for (size_t i = 0; i < indexAccessor.count; i++) {
					Vertex vertex = context.vertices[indices32[i]];
					context.indices.push_back(uniqueVertices[vertex]);
				}
			} else if (indexAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE) {
				const uint8_t* indices8 = reinterpret_cast<const uint8_t*>(indexData);
				for (size_t i = 0; i < indexAccessor.count; i++) {
					Vertex vertex = context.vertices[indices8[i]];
					context.indices.push_back(uniqueVertices[vertex]);
				}
			}
		}
	}
}
