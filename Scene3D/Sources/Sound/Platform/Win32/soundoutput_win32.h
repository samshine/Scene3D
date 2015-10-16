
#pragma once

#include "../../soundoutput_impl.h"
#include <mmdeviceapi.h>
#include <audioclient.h>

class SoundOutput_Win32 : public SoundOutputImpl
{
public:
	SoundOutput_Win32(int mixing_frequency, int mixing_latency = 50);
	~SoundOutput_Win32();

	void silence() override;
	int get_fragment_size() override;
	void write_fragment(float *data) override;
	void wait() override;

private:
	uicore::ComPtr<IMMDevice> mmdevice;
	uicore::ComPtr<IAudioClient> audio_client;
	uicore::ComPtr<IAudioRenderClient> audio_render_client;
	uicore::DataBufferPtr next_fragment;
	HANDLE audio_buffer_ready_event;
	bool is_playing;
	UINT32 fragment_size;
	int wait_timeout;
	int write_pos;
};
