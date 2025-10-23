/*
 * Copyright (c) 2025 Johannes Elsing
 *
 * Licensed under the Creative Commons Attribution-NonCommercial 4.0 International License.
 * You may not use this work for commercial purposes.
 * You must give appropriate credit and indicate if changes were made.
 * Full license: https://creativecommons.org/licenses/by-nc/4.0/legalcode
 */

#pragma once

// === Standard Library === //
#include <iostream>
#include <stdexcept>
#include <vector>
#include <string>
#include <memory>
#include <optional>
#include <filesystem>
#include <fstream>
#include <map>
#include <utility>
#include <chrono>

// === Third-party Libraries === //
#include "stb_image.h"

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_vulkan.h"

#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
