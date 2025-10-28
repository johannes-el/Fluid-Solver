#define TINYGLTF_IMPLEMENTATION
#define TINYGLTF_USE_STB_IMAGE_WRITE
#include <tiny_gltf.h>

#include "vulkan/vk_context.hpp"
#include "vulkan/vk_vertex.hpp"

#include "includes.hpp"
#include "vulkan/vk_model.hpp"

void loadModel(VkContext& context, const std::string model_path) {
	tinygltf::Model model;
	tinygltf::TinyGLTF loader;
	std::string err;
	std::string warn;

	bool ret = loader.LoadASCIIFromFile(&model, &err, &warn, model_path);
	if (!warn.empty()) std::cout << "glTF warning: " << warn << std::endl;
	if (!err.empty()) std::cout << "glTF error: " << err << std::endl;
	if (!ret) throw std::runtime_error("Failed to load glTF model");

	std::unordered_map<Vertex, uint32_t> uniqueVertices{};

	for (const auto& mesh : model.meshes) {
		for (const auto& primitive : mesh.primitives) {
			// Accessors and buffers
			const tinygltf::Accessor& posAccessor = model.accessors[primitive.attributes.at("POSITION")];
			const tinygltf::BufferView& posView = model.bufferViews[posAccessor.bufferView];
			const tinygltf::Buffer& posBuffer = model.buffers[posView.buffer];

			bool hasTexCoords = primitive.attributes.find("TEXCOORD_0") != primitive.attributes.end();
			const tinygltf::Accessor* texAccessor = nullptr;
			const tinygltf::BufferView* texView = nullptr;
			const tinygltf::Buffer* texBuffer = nullptr;
			if (hasTexCoords) {
				texAccessor = &model.accessors[primitive.attributes.at("TEXCOORD_0")];
				texView = &model.bufferViews[texAccessor->bufferView];
				texBuffer = &model.buffers[texView->buffer];
			}

			// 1. Build all vertices
			for (size_t i = 0; i < posAccessor.count; ++i) {
				Vertex vertex{};
				const float* pos = reinterpret_cast<const float*>(
					&posBuffer.data[posView.byteOffset + posAccessor.byteOffset + i * 12]
					);
				vertex.pos = {pos[0], pos[1], pos[2]};

				if (hasTexCoords) {
					const float* uv = reinterpret_cast<const float*>(
						&texBuffer->data[texView->byteOffset + texAccessor->byteOffset + i * 8]
						);
					vertex.texCoord = {uv[0], 1.0f - uv[1]};
				} else {
					vertex.texCoord = {0.0f, 0.0f};
				}

				vertex.color = {1.0f, 1.0f, 1.0f};

				if (!uniqueVertices.contains(vertex)) {
					uniqueVertices[vertex] = static_cast<uint32_t>(context.vertices.size());
					context.vertices.push_back(vertex);
				}
			}

			// 2. Map indices safely
			const tinygltf::Accessor& indexAccessor = model.accessors[primitive.indices];
			const tinygltf::BufferView& indexView = model.bufferViews[indexAccessor.bufferView];
			const tinygltf::Buffer& indexBuffer = model.buffers[indexView.buffer];
			const unsigned char* indexData = &indexBuffer.data[indexView.byteOffset + indexAccessor.byteOffset];

			auto pushIndex = [&](size_t idx) {
				Vertex vertex{};
				const float* pos = reinterpret_cast<const float*>(
					&posBuffer.data[posView.byteOffset + posAccessor.byteOffset + idx * 12]
					);
				vertex.pos = {pos[0], pos[1], pos[2]};

				if (hasTexCoords) {
					const float* uv = reinterpret_cast<const float*>(
						&texBuffer->data[texView->byteOffset + texAccessor->byteOffset + idx * 8]
						);
					vertex.texCoord = {uv[0], 1.0f - uv[1]};
				} else {
					vertex.texCoord = {0.0f, 0.0f};
				}

				vertex.color = {1.0f, 1.0f, 1.0f};
				context.indices.push_back(uniqueVertices[vertex]);
			};

			switch (indexAccessor.componentType) {
			case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE: {
				const uint8_t* indices = reinterpret_cast<const uint8_t*>(indexData);
				for (size_t i = 0; i < indexAccessor.count; ++i) pushIndex(indices[i]);
				break;
			}
			case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT: {
				const uint16_t* indices = reinterpret_cast<const uint16_t*>(indexData);
				for (size_t i = 0; i < indexAccessor.count; ++i) pushIndex(indices[i]);
				break;
			}
			case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT: {
				const uint32_t* indices = reinterpret_cast<const uint32_t*>(indexData);
				for (size_t i = 0; i < indexAccessor.count; ++i) pushIndex(indices[i]);
				break;
			}
			default:
				throw std::runtime_error("Unsupported index component type");
			}
		}
	}
}
