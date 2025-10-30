#include "gui/imgui.hpp"
#include "vulkan/vk_command.hpp"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"
#include "vulkan/vulkan.hpp"
#include <vulkan/vulkan_core.h>

ImGuiVulkanUtil::ImGuiVulkanUtil(vk::Device& device,
				vk::PhysicalDevice physicalDevice,
				vk::Queue& graphicsQueue,
				uint32_t graphicsQueueFamily)
	: device(device),
	  physicalDevice(physicalDevice),
	  graphicsQueue(graphicsQueue),
	  graphicsQueueFamily(graphicsQueueFamily)
{
	renderingInfo.colorAttachmentCount = 1;
	renderingInfo.pColorAttachmentFormats = &colorFormat;
}

ImGuiVulkanUtil::~ImGuiVulkanUtil() = default;

void ImGuiVulkanUtil::init(VkContext& context, float width, float height)
{
	vk::DescriptorPoolSize pool_sizes[] = {
		{ vk::DescriptorType::eSampler, 1000 },
		{ vk::DescriptorType::eCombinedImageSampler, 1000 },
		{ vk::DescriptorType::eSampledImage, 1000 },
		{ vk::DescriptorType::eStorageImage, 1000 },
		{ vk::DescriptorType::eUniformTexelBuffer, 1000 },
		{ vk::DescriptorType::eStorageTexelBuffer, 1000 },
		{ vk::DescriptorType::eUniformBuffer, 1000 },
		{ vk::DescriptorType::eStorageBuffer, 1000 },
		{ vk::DescriptorType::eUniformBufferDynamic, 1000 },
		{ vk::DescriptorType::eStorageBufferDynamic, 1000 },
		{ vk::DescriptorType::eInputAttachment, 1000 }
	};

	vk::DescriptorPoolCreateInfo pool_info{};
	pool_info.flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet;
	pool_info.maxSets = 1000 * IM_ARRAYSIZE(pool_sizes);
	pool_info.poolSizeCount = IM_ARRAYSIZE(pool_sizes);
	pool_info.pPoolSizes = pool_sizes;

	descriptorPool = device.createDescriptorPool(pool_info);
	context.imguiPool = descriptorPool;

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	ImGui_ImplGlfw_InitForVulkan(context.window, true);

	VkPipelineRenderingCreateInfoKHR pipelineRendering{};
	pipelineRendering.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR;
	pipelineRendering.colorAttachmentCount = 1;
	pipelineRendering.pColorAttachmentFormats = reinterpret_cast<VkFormat*>(&context.swapChainImageFormat);
	pipelineRendering.depthAttachmentFormat = VK_FORMAT_D32_SFLOAT;

	ImGui_ImplVulkan_PipelineInfo pipelineInfo{};
	pipelineInfo.PipelineRenderingCreateInfo = pipelineRendering;
	pipelineInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;

	ImGui_ImplVulkan_InitInfo info{};
	info.Instance = context.instance;
	info.PhysicalDevice = physicalDevice;
	info.Device = device;
	info.QueueFamily = graphicsQueueFamily;
	info.Queue = graphicsQueue;
	info.DescriptorPool = descriptorPool;
	info.MinImageCount = 2;
	info.ImageCount = static_cast<uint32_t>(context.swapChainImages.size());
	info.CheckVkResultFn = [](VkResult err){ assert(err == VK_SUCCESS); };
	info.UseDynamicRendering = true;
	info.PipelineInfoMain = pipelineInfo;

	ImGui_ImplVulkan_Init(&info);

	io.DisplaySize = ImVec2(width, height);
	io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);

	vulkanStyle = ImGui::GetStyle();
	vulkanStyle.Colors[ImGuiCol_TitleBg] = ImVec4(1.0f,0.0f,0.0f,0.6f);
	vulkanStyle.Colors[ImGuiCol_TitleBgActive] = ImVec4(1.0f,0.0f,0.0f,0.8f);
	vulkanStyle.Colors[ImGuiCol_MenuBarBg] = ImVec4(1.0f,0.0f,0.0f,0.4f);
	vulkanStyle.Colors[ImGuiCol_Header] = ImVec4(1.0f,0.0f,0.0f,0.4f);
	vulkanStyle.Colors[ImGuiCol_CheckMark] = ImVec4(0.0f,1.0f,0.0f,1.0f);

	setStyle(0);
}


void ImGuiVulkanUtil::setStyle(uint32_t index)
{
	ImGuiStyle& style = ImGui::GetStyle();

	switch (index) {
	case 0: style = vulkanStyle; break;
	case 1: ImGui::StyleColorsClassic(); break;
	case 2: ImGui::StyleColorsDark(); break;
	case 3: ImGui::StyleColorsLight(); break;
	}
}

void ImGuiVulkanUtil::initResources() {}

bool ImGuiVulkanUtil::newFrame()
{
	ImGui_ImplVulkan_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	ImGui::Begin("Vulkan ImGui Demo");
	ImGui::Text("");
	ImGui::End();

	ImGui::Render();
	return true;
}

void ImGuiVulkanUtil::updateBuffers() {}

void ImGuiVulkanUtil::drawFrame(vk::CommandBuffer& commandBuffer)
{
	ImDrawData* drawData = ImGui::GetDrawData();
	if (!drawData || drawData->CmdListsCount == 0)
		return;

	ImGui_ImplVulkan_RenderDrawData(drawData, commandBuffer.operator VkCommandBuffer());
}
