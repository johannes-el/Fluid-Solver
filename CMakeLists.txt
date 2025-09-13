cmake_minimum_required(VERSION 4.0)
project(VulkanProject LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

include_directories(include)

find_package(Vulkan REQUIRED)

add_executable(VulkanApp
    src/main.cpp

    # Vulkan source files
    src/vulkan/vk_context.cpp
    src/vulkan/vk_instance.cpp
    src/vulkan/vk_device.cpp
)

target_include_directories(VulkanApp PRIVATE ${Vulkan_INCLUDE_DIRS})
target_link_libraries(VulkanApp PRIVATE ${Vulkan_LIBRARIES} glfw)
