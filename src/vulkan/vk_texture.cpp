#include "vulkan/vk_context.hpp"
#include "vulkan/vk_texture.hpp"
#include "vulkan/vk_buffer.hpp"
#include "vulkan/vk_image.hpp"
#include "vulkan/vulkan.hpp"

void createTextureImage(VkContext& context)
{
	int texWidth, texHeight, texChannels;
	stbi_uc* pixels = stbi_load("../models/textures/mrletsgo_Albedo.tga.png", &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
	vk::DeviceSize imageSize = texWidth * texHeight * 4;

	if (!pixels) {
		throw std::runtime_error("Failed to load texture image!");
	}

	vk::Buffer stagingBuffer{};
	vk::DeviceMemory stagingBufferMemory{};

	createBuffer(context, imageSize, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, stagingBuffer, stagingBufferMemory);

	void* data = context.device.mapMemory(stagingBufferMemory, 0, imageSize);
	memcpy(data, pixels, static_cast<size_t>(imageSize));
	context.device.unmapMemory(stagingBufferMemory);

	stbi_image_free(pixels);

	createImage(context, texWidth, texHeight, vk::Format::eR8G8B8A8Srgb, vk::ImageTiling::eOptimal, vk::ImageUsageFlagBits::eTransferDst
		| vk::ImageUsageFlagBits::eSampled, vk::MemoryPropertyFlagBits::eDeviceLocal, context.textureImage, context.textureImageMemory);

	transitionImageLayout(context, context.textureImage, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal);
	copyBufferToImage(context, stagingBuffer, context.textureImage, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));
	transitionImageLayout(context, context.textureImage, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal);
}


void createTextureSampler(VkContext& context)
{
	vk::PhysicalDeviceProperties properties = context.gpu.getProperties();

	vk::SamplerCreateInfo samplerInfo {
		.magFilter = vk::Filter::eLinear,
		.minFilter = vk::Filter::eLinear,
		.mipmapMode = vk::SamplerMipmapMode::eLinear,
		.addressModeU = vk::SamplerAddressMode::eRepeat,
		.addressModeV = vk::SamplerAddressMode::eRepeat,
		.addressModeW = vk::SamplerAddressMode::eRepeat,
		.anisotropyEnable = vk::True,
		.maxAnisotropy = properties.limits.maxSamplerAnisotropy,
		.compareEnable = vk::False,
		.compareOp = vk::CompareOp::eAlways
	};

	context.textureSampler = context.device.createSampler(samplerInfo);
}
