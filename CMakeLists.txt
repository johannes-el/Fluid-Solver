cmake_minimum_required(VERSION 3.15)
project(VulkanProject LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# === Paths ===
include_directories(include)

# === Find Vulkan ===
find_package(Vulkan REQUIRED)

# === Optional: Find GLFW system-wide ===
find_package(glfw3 3.3 REQUIRED)

# === ImGui ===
add_library(imgui
    external/imgui/imgui.cpp
    external/imgui/imgui_draw.cpp
    external/imgui/imgui_tables.cpp
    external/imgui/imgui_widgets.cpp
    external/imgui/imgui_demo.cpp # optional but helpful for testing
    external/imgui/backends/imgui_impl_glfw.cpp
    external/imgui/backends/imgui_impl_vulkan.cpp
)
# === tinyobjloader ===
add_subdirectory(external/tinyobjloader)

add_library(tinygltf INTERFACE)
target_include_directories(tinygltf INTERFACE external/tinygltf)
target_compile_definitions(tinygltf INTERFACE TINYGLTF_NO_STB)

# === VulkanMemoryAllocator ===
add_library(vma STATIC
    external/VulkanMemoryAllocator/src/VmaUsage.cpp
)

target_include_directories(vma PUBLIC
    external/VulkanMemoryAllocator/include
)

target_include_directories(imgui PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/external/imgui
    ${CMAKE_CURRENT_SOURCE_DIR}/external/imgui/backends
)

# === VulkanApp Executable ===
add_executable(VulkanApp
    src/main.cpp
    src/FileIO.cpp
    src/stb_image.cpp
    src/stb_image_write.cpp

    # Vulkan modules
    src/vulkan/vk_context.cpp
    src/vulkan/vk_instance.cpp
    src/vulkan/vk_device.cpp
    src/vulkan/vk_swapchain.cpp
    src/vulkan/vk_image.cpp
    src/vulkan/vk_pipeline.cpp
    src/vulkan/vk_command.cpp
    src/vulkan/vk_sync.cpp
    src/vulkan/vk_buffer.cpp
    src/vulkan/vk_descriptor.cpp
    src/vulkan/vk_texture.cpp
    src/vulkan/vk_model.cpp

    # GUI integration
    src/gui/imgui.cpp

    # Audio
    src/audio/audio.cpp
)

# === Include Directories ===
target_include_directories(VulkanApp PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/external/stb
    ${Vulkan_INCLUDE_DIRS}
)

# === Preprocessor Definitions ===
target_compile_definitions(VulkanApp PUBLIC
    VULKAN_HPP_NO_STRUCT_CONSTRUCTORS=1
)

# === Link Libraries ===
target_link_libraries(VulkanApp PRIVATE
    ${Vulkan_LIBRARIES}
    glfw
    imgui
    vma
    tinygltf
    X11
)

# === Optional: Group folders in IDEs ===
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src PREFIX "Source" FILES
    src/main.cpp
    src/FileIO.cpp
    src/stb_image.cpp
    src/stb_image_write.cpp
    src/vulkan/vk_context.cpp
    src/vulkan/vk_instance.cpp
    src/vulkan/vk_device.cpp
    src/vulkan/vk_swapchain.cpp
    src/vulkan/vk_image.cpp
    src/vulkan/vk_pipeline.cpp
    src/vulkan/vk_command.cpp
    src/vulkan/vk_sync.cpp
    src/vulkan/vk_vertex.cpp
    src/vulkan/vk_buffer.cpp
    src/vulkan/vk_descriptor.cpp
    src/gui/imgui.cpp
)
