#pragma once

#include "includes.hpp"

struct Vertex {
	glm::vec3 pos;
	glm::vec3 color;
	glm::vec2 texCoord;

	static vk::VertexInputBindingDescription getBindingDescription() {
		return { 0, sizeof(Vertex), vk::VertexInputRate::eVertex };
	}

	static std::array<vk::VertexInputAttributeDescription, 3> getAttributeDescriptions() {
		return {
			vk::VertexInputAttributeDescription(0, 0, vk::Format::eR32G32B32Sfloat, offsetof(Vertex, pos)),
			vk::VertexInputAttributeDescription(1, 0, vk::Format::eR32G32B32Sfloat, offsetof(Vertex, color)),
			vk::VertexInputAttributeDescription(2, 0, vk::Format::eR32G32Sfloat,  offsetof(Vertex, texCoord))
		};
	}

	bool operator==(const Vertex& other) const {
		return pos == other.pos && color == other.color && texCoord == other.texCoord;
	}
};

namespace std {
	template<>
	struct hash<glm::vec2> {
		size_t operator()(const glm::vec2& v) const {
			size_t h1 = std::hash<float>()(v.x);
			size_t h2 = std::hash<float>()(v.y);
			return h1 ^ (h2 << 1);
		}
	};

	template<>
	struct hash<glm::vec3> {
		size_t operator()(const glm::vec3& v) const {
			size_t h1 = std::hash<float>()(v.x);
			size_t h2 = std::hash<float>()(v.y);
			size_t h3 = std::hash<float>()(v.z);
			return ((h1 ^ (h2 << 1)) >> 1) ^ (h3 << 1);
		}
	};

	template<>
	struct hash<Vertex> {
		size_t operator()(const Vertex& v) const {
			size_t h1 = hash<glm::vec3>()(v.pos);
			size_t h2 = hash<glm::vec3>()(v.color);
			size_t h3 = hash<glm::vec2>()(v.texCoord);
			return ((h1 ^ (h2 << 1)) >> 1) ^ (h3 << 1);
		}
	};
}
