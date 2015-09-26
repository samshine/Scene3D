
#pragma once

#include "speaker_position.h"
#include "sound_mixing_buffers_data.h"

struct SoundFormatConversion
{
	static void from_ieee_float(SoundMixingBuffersData &output, void *input, SpeakerPositionMask input_speakers, unsigned int sample_count);
	static void from_16bit_le(SoundMixingBuffersData &output, void *input, SpeakerPositionMask input_speakers, unsigned int sample_count);
	static void from_16bit_be(SoundMixingBuffersData &output, void *input, SpeakerPositionMask input_speakers, unsigned int sample_count);
	static void from_8bit_signed(SoundMixingBuffersData &output, void *input, SpeakerPositionMask input_speakers, unsigned int sample_count);
	static void from_8bit_unsigned(SoundMixingBuffersData &output, void *input, SpeakerPositionMask input_speakers, unsigned int sample_count);

	static void to_ieee_float(void **output, SpeakerPositionMask output_speakers, const SoundMixingBuffersData &input, unsigned int sample_count);
	static void to_16bit_le(void **output, SpeakerPositionMask output_speakers, const SoundMixingBuffersData &input, unsigned int sample_count);
	static void to_16bit_be(void **output, SpeakerPositionMask output_speakers, const SoundMixingBuffersData &input, unsigned int sample_count);
	static void to_8bit_signed(void **output, SpeakerPositionMask output_speakers, const SoundMixingBuffersData &input, unsigned int sample_count);
	static void to_8bit_unsigned(void **output, SpeakerPositionMask output_speakers, const SoundMixingBuffersData &input, unsigned int sample_count);

	//	static void resample(SoundMixingBuffersData &output, int sample_count_output, const SoundMixingBuffersData &input, unsigned int sample_count_input);

private:
	static unsigned int find_output_speakers(SoundMixingBuffersData &output, SpeakerPositionMask input_speakers, float **output_speakers);
	static unsigned int find_input_speakers(const SoundMixingBuffersData &input, SpeakerPositionMask output_speakers, float **input_speakers);
	static void clear_unused_speakers(SoundMixingBuffersData &output, SpeakerPositionMask input_speakers, unsigned int sample_count);
};
