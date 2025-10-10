#pragma once

#include "includes.hpp"
#include "miniaudio.h"

namespace Audio
{
	struct AudioContext {
		ma_engine engine;
	};

	ma_result init(AudioContext& context);
	ma_result playSound(AudioContext&, std::filesystem::path& path);
	void playMusic(AudioContext&, std::filesystem::path& path, bool loop = true);
	void shutdown(AudioContext&);
} // namespace Audio
