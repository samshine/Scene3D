
#pragma once

#include "soundprovider.h"

class SoundProvider_WaveImpl;

class SoundProvider_Wave : public SoundProvider
{
public:
	SoundProvider_Wave(const std::string &filename, bool stream = false);
	SoundProvider_Wave(uicore::IODevice &file, bool stream = false);
	virtual ~SoundProvider_Wave();

	virtual SoundProvider_Session *begin_session() override;
	virtual void end_session(SoundProvider_Session *session) override;

private:
	std::shared_ptr<SoundProvider_WaveImpl> impl;

	friend class SoundProvider_Wave_Session;
};
