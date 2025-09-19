/*
 * Copyright (c) 2025 Johannes Elsing
 *
 * Licensed under the Creative Commons Attribution-NonCommercial 4.0 International License.
 * You may not use this work for commercial purposes.
 * You must give appropriate credit and indicate if changes were made.
 * Full license: https://creativecommons.org/licenses/by-nc/4.0/legalcode
 */

#include "vulkan/vk_pipeline.hpp"
#include "FileIO.hpp"

static std::vector<vk::DynamicState> dynamicStates = {
	vk::DynamicState::eViewport,
	vk::DynamicState::eScissor
};

[[nodiscard]] vk::ShaderModule createShaderModule(VkContext& context, const std::vector<char>& code) {
	vk::ShaderModuleCreateInfo createInfo{};
	createInfo.codeSize = code.size() * sizeof(char);
	createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

	vk::ShaderModule shaderModule{};

	return shaderModule;
}
void createGraphicsPipeline(VkContext& context)
{
	vk::ShaderModule shaderModule = createShaderModule(context, readFile("./../shaders/vert.spv"));

	vk::PipelineShaderStageCreateInfo vertShaderStageInfo {
		.stage = vk::ShaderStageFlagBits::eVertex,
		.module = shaderModule,
		.pName = "vertMain",
	};

	vk::PipelineShaderStageCreateInfo fragShaderStageInfo {
		.stage = vk::ShaderStageFlagBits::eFragment,
		.module = shaderModule,
		.pName = "fragMain"
	};

	vk::PipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};

	vk::PipelineDynamicStateCreateInfo dynamicState {
		.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size()),
		.pDynamicStates = dynamicStates.data()
	};

	vk::PipelineVertexInputStateCreateInfo vertexInputInfo;
	vk::PipelineInputAssemblyStateCreateInfo inputAssembly {
		.topology = vk::PrimitiveTopology::eTriangleList
	};

	vk::PipelineRenderingCreateInfo pipelineRenderingCreateInfo {
		.colorAttachmentCount = 1,
		.pColorAttachmentFormats = &context.swapChainImageFormat
	};

	vk::PipelineViewportStateCreateInfo viewportState({}, {}, {}, 1);

	vk::PipelineRasterizationStateCreateInfo rasterizer({}, {}, {}, vk::False);

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

	// context.pipelineLayout = vk::PipelineLayout(context.device, pipelineLayoutInfo);

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
}

