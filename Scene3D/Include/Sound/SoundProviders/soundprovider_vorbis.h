
#pragma once

#include "soundprovider.h"

class SoundProvider_Vorbis_Impl;

class SoundProvider_Vorbis : public SoundProvider
{
public:
	SoundProvider_Vorbis(const std::string &filename, bool stream = false);
	SoundProvider_Vorbis(uicore::IODevice &file, bool stream = false);
	virtual ~SoundProvider_Vorbis();

	virtual SoundProvider_Session *begin_session() override;
	virtual void end_session(SoundProvider_Session *session) override;

private:
	std::shared_ptr<SoundProvider_Vorbis_Impl> impl;

	friend class SoundProvider_Vorbis_Session;
};
