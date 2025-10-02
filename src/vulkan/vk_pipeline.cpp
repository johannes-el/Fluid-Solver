/*
 * Copyright (c) 2025 Johannes Elsing
 *
 * Licensed under the Creative Commons Attribution-NonCommercial 4.0 International License.
 * You may not use this work for commercial purposes.
 * You must give appropriate credit and indicate if changes were made.
 * Full license: https://creativecommons.org/licenses/by-nc/4.0/legalcode
 */

#include "vulkan/vk_pipeline.hpp"
#include "vulkan/vk_vertex.hpp"
#include "FileIO.hpp"

static std::vector<vk::DynamicState> dynamicStates = {
	vk::DynamicState::eViewport,
	vk::DynamicState::eScissor
};

[[nodiscard]] vk::ShaderModule createShaderModule(VkContext& context, const std::vector<char>& code) {
	vk::ShaderModuleCreateInfo createInfo{};
	createInfo.codeSize = code.size() * sizeof(char);
	createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

	return context.device.createShaderModule(createInfo);
}
void createGraphicsPipeline(VkContext& context)
{
	context.shaderModule = createShaderModule(context, readFile("./../shaders/slang.spv"));

	vk::PipelineShaderStageCreateInfo vertShaderStageInfo {
		.stage = vk::ShaderStageFlagBits::eVertex,
		.module = context.shaderModule,
		.pName = "vertMain"
	};

	vk::PipelineShaderStageCreateInfo fragShaderStageInfo {
		.stage = vk::ShaderStageFlagBits::eFragment,
		.module = context.shaderModule,
		.pName = "fragMain"
	};

	vk::PipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};

	vk::PipelineDynamicStateCreateInfo dynamicState {
		.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size()),
		.pDynamicStates = dynamicStates.data()
	};

	auto bindingDescription = Vertex::getBindingDescription();
	auto attributeDescriptions = Vertex::getAttributeDescriptions();

	vk::PipelineVertexInputStateCreateInfo vertexInputInfo {
		.vertexBindingDescriptionCount = 1,
		.pVertexBindingDescriptions = &bindingDescription,
		.vertexAttributeDescriptionCount = attributeDescriptions.size(),
		.pVertexAttributeDescriptions = attributeDescriptions.data()
	};

	vk::PipelineInputAssemblyStateCreateInfo inputAssembly {
		.topology = vk::PrimitiveTopology::eTriangleList
	};

	vk::PipelineRenderingCreateInfo pipelineRenderingCreateInfo {
		.colorAttachmentCount = 1,
		.pColorAttachmentFormats = &context.swapChainImageFormat
	};

	vk::PipelineViewportStateCreateInfo viewportState = vk::PipelineViewportStateCreateInfo{}
		.setViewportCount(1)
		.setScissorCount(1);

	vk::PipelineRasterizationStateCreateInfo rasterizer{};
	rasterizer.depthClampEnable = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode = vk::PolygonMode::eFill;
	rasterizer.lineWidth = 1.0f;
	rasterizer.cullMode = vk::CullModeFlagBits::eBack;
	rasterizer.frontFace = vk::FrontFace::eClockwise;
	rasterizer.depthBiasEnable = VK_FALSE;

	vk::PipelineMultisampleStateCreateInfo multisampling {
		.rasterizationSamples = vk::SampleCountFlagBits::e1,
		.sampleShadingEnable = vk::False,
	};

	vk::PipelineColorBlendAttachmentState colorBlendAttachment;
	colorBlendAttachment.colorWriteMask = vk::ColorComponentFlagBits::eR
		| vk::ColorComponentFlagBits::eG
		| vk::ColorComponentFlagBits::eB
		| vk::ColorComponentFlagBits::eA;

	vk::PipelineColorBlendStateCreateInfo colorBlending {
		.logicOpEnable = vk::False,
		.logicOp = vk::LogicOp::eCopy,
		.attachmentCount = 1,
		.pAttachments = &colorBlendAttachment
	};

	vk::PipelineLayoutCreateInfo pipelineLayoutInfo {
		.setLayoutCount = 0,
		.pushConstantRangeCount = 0
	};

	context.pipelineLayout = context.device.createPipelineLayout(pipelineLayoutInfo);

	vk::GraphicsPipelineCreateInfo pipelineInfo {
		.pNext = &pipelineRenderingCreateInfo,
		.stageCount = 2,
		.pStages = shaderStages,
		.pVertexInputState = &vertexInputInfo,
		.pInputAssemblyState = &inputAssembly,
		.pViewportState = &viewportState,
		.pRasterizationState = &rasterizer,
		.pMultisampleState = &multisampling,
		.pColorBlendState = &colorBlending,
		.pDynamicState = &dynamicState,
		.layout = context.pipelineLayout,
		.renderPass = nullptr
	};

	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
	pipelineInfo.basePipelineIndex = -1;

	auto [result, pipeline] = context.device.createGraphicsPipeline(nullptr, pipelineInfo);

	if (result != vk::Result::eSuccess) {
		throw std::runtime_error("Failed to create graphics pipeline!");
	}
	context.graphicsPipeline = pipeline;
}

