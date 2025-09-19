/*
 * Copyright (c) 2025 Johannes Elsing
 *
 * Licensed under the Creative Commons Attribution-NonCommercial 4.0 International License.
 * You may not use this work for commercial purposes.
 * You must give appropriate credit and indicate if changes were made.
 * Full license: https://creativecommons.org/licenses/by-nc/4.0/legalcode
 */

//////////////////////////////////////////////////////////////////////////
/*

  This is an implementation of a fluid solver for
  a bachelor project in Computer Graphics.

 */
//////////////////////////////////////////////////////////////////////////

#include "vulkan/vk_context.hpp"
#include "vulkan/vk_instance.hpp"
#include "app_config.hpp"

void printUsage()
{
    std::cerr << "Usage: program [--width N] [--height N] [--title NAME]\n";
}

/* Parse command line arguments. */
AppConfig parseArgs(int argc, char** argv)
{
    AppConfig config;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        if (arg == "--width" && i + 1 < argc) {
            config.width = std::stoi(argv[++i]);
        }
        else if (arg == "--height" && i + 1 < argc) {
            config.height = std::stoi(argv[++i]);
        }
        else if (arg == "--title" && i + 1 < argc) {
            config.title = argv[++i];
        }
        else {
            printUsage();
	    throw std::runtime_error("Unknown argument: " + arg);
        }
    }

    return config;
}

int main(int argc, char** argv)
{
	AppConfig config{};

	try {
		config = parseArgs(argc, argv);
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << '\n';
		return -1;
	}

	VkContext context{};
	initWindow(context, config);
	initVulkan(context);

	run(context);

	cleanup(context);
}
