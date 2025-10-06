#include "gui/imgui.hpp"

void initImGUI(VkContext& context)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();

	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // IF using Docking Branch

	ImGui_ImplVulkan_InitInfo init_info = {};
	init_info.Instance = context.instance;
	init_info.PhysicalDevice = context.gpu;
	init_info.Device = context.device;
	init_info.Queue = context.graphicsQueue;
	init_info.PipelineCache = VK_NULL_HANDLE;
	// init_info.DescriptorPool = context.imguiDescriptorPool;
	// init_info.Subpass = 0;
	init_info.MinImageCount = MAX_FRAMES_IN_FLIGHT;
	init_info.ImageCount = static_cast<uint32_t>(context.swapChainImages.size());
	// init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
	init_info.Allocator = nullptr;
	init_info.CheckVkResultFn = nullptr;
	// init_info.RenderPass = context.renderPass;

	ImGui_ImplVulkan_Init(&init_info);
}
