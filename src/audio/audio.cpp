#define MINIAUDIO_IMPLEMENTATION

#include "audio/miniaudio.h"
#include "audio/audio.hpp"
#include "includes.hpp"

namespace Audio
{
	ma_result init(AudioContext& context)
	{
		ma_result result = ma_engine_init(nullptr, &context.engine);
		if (result != MA_SUCCESS) {
			return result;
		}

		return MA_SUCCESS;
	}

	ma_result playSound(AudioContext& context, std::filesystem::path& path)
	{
		// Implementation missing.
		return MA_SUCCESS;
	}

	void playMusic(AudioContext& context, std::filesystem::path& path, bool loop)
	{
		ma_sound* music = new ma_sound();
		ma_result ma_result = ma_sound_init_from_file(
			&context.engine,
			path.c_str(),
			MA_SOUND_FLAG_STREAM,
			nullptr,
			nullptr,
			music
		);

		if (ma_result != MA_SUCCESS) {
			std::cerr << "Failed to load music: "  << path.c_str() << std::endl;
			delete music;
		}

		ma_sound_set_looping(music, loop ? MA_TRUE : MA_FALSE);
		ma_sound_start(music);
	}

	void shutdown(AudioContext& context)
	{
		ma_engine_uninit(&context.engine);
	}
}
