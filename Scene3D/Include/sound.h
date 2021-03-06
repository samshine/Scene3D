
#pragma once

#include "Sound/sound.h"
#include "Sound/soundoutput.h"
#include "Sound/soundoutput_description.h"
#include "Sound/soundformat.h"
#include "Sound/soundbuffer.h"
#include "Sound/soundbuffer_session.h"
#include "Sound/sound_sse.h"
#include "Sound/sound_cache.h"
#include "Sound/soundprovider.h"
#include "Sound/soundprovider_session.h"
#include "Sound/music_player.h"
#include "Sound/AudioWorld/audio_object.h"
#include "Sound/AudioWorld/audio_world.h"

#if defined(_MSC_VER)
	#pragma comment(lib, "winmm.lib")
#endif
