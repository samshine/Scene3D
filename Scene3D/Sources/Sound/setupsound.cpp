/*
**  ClanLib SDK
**  Copyright (c) 1997-2015 The ClanLib Team
**
**  This software is provided 'as-is', without any express or implied
**  warranty.  In no event will the authors be held liable for any damages
**  arising from the use of this software.
**
**  Permission is granted to anyone to use this software for any purpose,
**  including commercial applications, and to alter it and redistribute it
**  freely, subject to the following restrictions:
**
**  1. The origin of this software must not be misrepresented; you must not
**     claim that you wrote the original software. If you use this software
**     in a product, an acknowledgment in the product documentation would be
**     appreciated but is not required.
**  2. Altered source versions must be plainly marked as such, and must not be
**     misrepresented as being the original software.
**  3. This notice may not be removed or altered from any source distribution.
**
**  Note: Some of the libraries ClanLib may link to may have additional
**  requirements or restrictions.
**
**  File Author(s):
**
**    Magnus Norddahl
**    Mark Page
*/

#include "precomp.h"
#include "setupsound.h"
#include "Sound/SoundProviders/soundprovider_wave.h"
#include "Sound/SoundProviders/soundprovider_vorbis.h"
#include "Sound/SoundProviders/soundprovider_type_register.h"

#define INCLUDED_FROM_SETUPVORBIS
#include "SoundProviders/stb_vorbis.h"

namespace uicore
{
	void SetupSound::start()
	{
		static bool first_time = true;
		if (first_time)
		{
			first_time = false;
			static auto providertype_wave = std::make_unique<SoundProviderType_Register<SoundProvider_Wave>>("wav");
			static auto providertype_ogg = std::make_unique<SoundProviderType_Register<SoundProvider_Vorbis>>("ogg");
		}
	}

	std::map<std::string, SoundProviderType *> *SetupSound::get_sound_provider_factory_types()
	{
		static std::map<std::string, SoundProviderType *> types;
		return &types;
	}
}

