#include "gui/imgui.hpp"
#include "vulkan/vk_command.hpp"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"
#include "vulkan/vulkan.hpp"

ImGuiVulkanUtil::ImGuiVulkanUtil(vk::Device& device, vk::PhysicalDevice physialDevice,
			vk::Queue& graphicsQueue, uint32_t graphicsQueueFamily)
	: device(device), physicalDevice(physialDevice),
	  graphicsQueue(graphicsQueue), graphicsQueueFamily(graphicsQueueFamily)
{
	renderingInfo.colorAttachmentCount = 1;
	vk::Format formats[] = { colorFormat };
	renderingInfo.pColorAttachmentFormats = &colorFormat;
}

ImGuiVulkanUtil::~ImGuiVulkanUtil()
{
	if (device) {
		device.waitIdle();
	}
}

void ImGuiVulkanUtil::init(VkContext& context, float width, float height)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

	io.DisplaySize = ImVec2(width, height);
	io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);

	vulkanStyle = ImGui::GetStyle();
	vulkanStyle.Colors[ImGuiCol_TitleBg] = ImVec4(1.0f, 0.0f, 0.0f, 0.6f);
	vulkanStyle.Colors[ImGuiCol_TitleBgActive] = ImVec4(1.0f, 0.0f, 0.0f, 0.8f);
	vulkanStyle.Colors[ImGuiCol_MenuBarBg] = ImVec4(1.0f, 0.0f, 0.0f, 0.4f);
	vulkanStyle.Colors[ImGuiCol_Header] = ImVec4(1.0f, 0.0f, 0.0f, 0.4f);
	vulkanStyle.Colors[ImGuiCol_CheckMark] = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);

	setStyle(0);
}


void ImGuiVulkanUtil::setStyle(uint32_t index)
{
	ImGuiStyle& style = ImGui::GetStyle();

	switch (index) {
	case 0:
		style = vulkanStyle;
		break;
	case 1:
		ImGui::StyleColorsClassic();
		break;
	case 2:
		ImGui::StyleColorsDark();
		break;
	case 3:
		ImGui::StyleColorsLight();
		break;
	}
}


void ImGuiVulkanUtil::initResources()
{
	ImGuiIO& io = ImGui::GetIO();
	unsigned char* fontData;
	int texWidth, texHeight;
	io.Fonts->GetTexDataAsRGBA32(&fontData, &texWidth, &texHeight);

	vk::Extent3D extent{};
	extent.width  = static_cast<uint32_t>(texWidth);
	extent.height = static_cast<uint32_t>(texHeight);
	extent.depth  = 1;

	vk::ImageCreateInfo imageInfo{};
	imageInfo.imageType   = vk::ImageType::e2D;
	imageInfo.format      = vk::Format::eR8G8B8A8Unorm;
	imageInfo.extent      = extent;
	imageInfo.mipLevels   = 1;
	imageInfo.arrayLayers = 1;
	imageInfo.samples     = vk::SampleCountFlagBits::e1;
	imageInfo.tiling      = vk::ImageTiling::eOptimal;
	imageInfo.usage       = vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled;

	fontImage = device.createImage(imageInfo);

	vk::ImageSubresourceRange subRange{};
	subRange.aspectMask     = vk::ImageAspectFlagBits::eColor;
	subRange.baseMipLevel   = 0;
	subRange.levelCount     = 1;
	subRange.baseArrayLayer = 0;
	subRange.layerCount     = 1;

	vk::ImageViewCreateInfo viewInfo{};
	viewInfo.image            = fontImage;
	viewInfo.viewType         = vk::ImageViewType::e2D;
	viewInfo.format           = vk::Format::eR8G8B8A8Unorm;
	viewInfo.subresourceRange = subRange;

	fontImageView = device.createImageView(viewInfo);
}


bool ImGuiVulkanUtil::newFrame()
{
	ImGui::NewFrame();

	ImGui::Begin("Vulkan ImGui Demo");
	ImGui::Text("Hello, Vulkan!");
	if (ImGui::Button("Click me")) {
		//
	}

	ImGui::End();
	ImGui::EndFrame();

	ImGui::Render();

	ImDrawData* drawData = ImGui::GetDrawData();

	if (drawData && drawData->CmdListsCount > 0) {
		if (drawData->TotalVtxCount > vertexCount || drawData->TotalIdxCount > indexCount) {
			needsUpdateBuffers = true;
			return true;
		}
	}

	return false;
}

void ImGuiVulkanUtil::updateBuffers()
{
	ImDrawData* drawData = ImGui::GetDrawData();
	if (!drawData || drawData->CmdListsCount == 0) {
		return;
	}

	vk::DeviceSize vertexBufferSize = drawData->TotalVtxCount * sizeof(ImDrawVert);
	vk::DeviceSize indexBufferSize = drawData->TotalIdxCount * sizeof(ImDrawIdx);

	if (drawData->TotalVtxCount > vertexCount) {
		vk::BufferCreateInfo info{};
		info.size = vertexBufferSize;
		// info.flags =
		vertexBuffer = device.createBuffer(info);
	}

	// vertexBuffer.unmap();
}

void ImGuiVulkanUtil::drawFrame(vk::CommandBuffer& commandBuffer)
{
	ImDrawData* drawData = ImGui::GetDrawData();
	if (!drawData || drawData->CmdListsCount == 0) {
		return;
	}

	vk::RenderingAttachmentInfo colorAttachment{};
	vk::RenderingInfo renderingInfo{};

	renderingInfo.renderArea = vk::Rect2D{{0, 0}, {static_cast<uint32_t>(drawData->DisplaySize.x),
			static_cast<uint32_t>(drawData->DisplaySize.y)}};
	renderingInfo.layerCount = 1;
	renderingInfo.colorAttachmentCount = 1;
	renderingInfo.pColorAttachments = &colorAttachment;

	commandBuffer.beginRendering(renderingInfo);
	commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline);

	vk::Viewport viewport{};
	viewport.width = drawData->DisplaySize.x;
	viewport.height = drawData->DisplaySize.y;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	commandBuffer.setViewport(0, viewport);
}
