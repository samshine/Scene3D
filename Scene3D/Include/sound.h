
#pragma once

#include "Sound/sound.h"
#include "Sound/soundoutput.h"
#include "Sound/soundoutput_description.h"
#include "Sound/soundformat.h"
#include "Sound/soundbuffer.h"
#include "Sound/soundbuffer_session.h"
#include "Sound/sound_sse.h"
#include "Sound/sound_cache.h"
#include "Sound/SoundProviders/soundprovider.h"
#include "Sound/SoundProviders/soundprovider_session.h"
#include "Sound/SoundProviders/soundprovider_type_register.h"
#include "Sound/SoundProviders/soundprovider_wave.h"
#include "Sound/SoundProviders/soundprovider_raw.h"
#include "Sound/SoundProviders/soundprovider_vorbis.h"
#include "Sound/AudioWorld/audio_definition.h"
#include "Sound/AudioWorld/audio_object.h"
#include "Sound/AudioWorld/audio_world.h"

#if defined(_MSC_VER)
	#pragma comment(lib, "winmm.lib")
#endif
