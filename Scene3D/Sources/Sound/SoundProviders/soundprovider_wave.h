
#pragma once

#include "Sound/soundprovider.h"
#include "Sound/soundformat.h"

class SoundProvider_Wave : public SoundProvider, public std::enable_shared_from_this<SoundProvider_Wave>
{
public:
	SoundProvider_Wave(const uicore::IODevicePtr &input, bool stream);

	std::shared_ptr<SoundProvider_Session> begin_session() override;

	uicore::DataBufferPtr data;
	SoundFormat format;
	int num_channels;
	int num_samples;
	int frequency;

private:
	uint32_t find_subchunk(const char *chunk, const uicore::IODevicePtr &source, uint32_t file_offset, uint32_t max_offset);
};
