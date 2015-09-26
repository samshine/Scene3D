
#include "precomp.h"
#include "sound_format_conversion.h"

using namespace uicore;

void SoundFormatConversion::from_ieee_float(SoundMixingBuffersData &output, void *input, SpeakerPositionMask input_speakers, unsigned int sample_count)
{
	float *output_speakers[32];
	unsigned int num_input_speakers = find_output_speakers(output, input_speakers, output_speakers);
	clear_unused_speakers(output, input_speakers, sample_count);

	unsigned int bytes_per_sample = sizeof(float)*num_input_speakers;
	char *input_pos = reinterpret_cast<char*>(input);
	for (unsigned int i = 0; i < sample_count; i++, input_pos += bytes_per_sample)
	{
		float *input_sample = reinterpret_cast<float*>(input_pos);
		for (unsigned int j = 0; j < num_input_speakers; j++)
		{
			if (output_speakers[j])
				output_speakers[j][i] = input_sample[j];
		}
	}
}

void SoundFormatConversion::from_16bit_le(SoundMixingBuffersData &output, void *input, SpeakerPositionMask input_speakers, unsigned int sample_count)
{
	float *output_speakers[32];
	unsigned int num_input_speakers = find_output_speakers(output, input_speakers, output_speakers);
	clear_unused_speakers(output, input_speakers, sample_count);

	unsigned int bytes_per_sample = sizeof(short)*num_input_speakers;
	char *input_pos = reinterpret_cast<char*>(input);
	for (unsigned int i = 0; i < sample_count; i++, input_pos += bytes_per_sample)
	{
		short *input_sample = reinterpret_cast<short*>(input_pos);
		for (unsigned int j = 0; j < num_input_speakers; j++)
		{
			if (output_speakers[j])
				output_speakers[j][i] = input_sample[j] / 32768.0f;
		}
	}
}

void SoundFormatConversion::from_16bit_be(SoundMixingBuffersData &output, void *input, SpeakerPositionMask input_speakers, unsigned int sample_count)
{
	throw Exception("16 bit big endian samples not implemented");
}

void SoundFormatConversion::from_8bit_signed(SoundMixingBuffersData &output, void *input, SpeakerPositionMask input_speakers, unsigned int sample_count)
{
	float *output_speakers[32];
	unsigned int num_input_speakers = find_output_speakers(output, input_speakers, output_speakers);
	clear_unused_speakers(output, input_speakers, sample_count);

	unsigned int bytes_per_sample = sizeof(char)*num_input_speakers;
	char *input_pos = reinterpret_cast<char*>(input);
	for (unsigned int i = 0; i < sample_count; i++, input_pos += bytes_per_sample)
	{
		char *input_sample = reinterpret_cast<char*>(input_pos);
		for (unsigned int j = 0; j < num_input_speakers; j++)
		{
			if (output_speakers[j])
				output_speakers[j][i] = input_sample[j] / 128.0f;
		}
	}
}

void SoundFormatConversion::from_8bit_unsigned(SoundMixingBuffersData &output, void *input, SpeakerPositionMask input_speakers, unsigned int sample_count)
{
	float *output_speakers[32];
	unsigned int num_input_speakers = find_output_speakers(output, input_speakers, output_speakers);
	clear_unused_speakers(output, input_speakers, sample_count);

	unsigned int bytes_per_sample = sizeof(char)*num_input_speakers;
	char *input_pos = reinterpret_cast<char*>(input);
	for (unsigned int i = 0; i < sample_count; i++, input_pos += bytes_per_sample)
	{
		unsigned char *input_sample = reinterpret_cast<unsigned char*>(input_pos);
		for (unsigned int j = 0; j < num_input_speakers; j++)
		{
			if (output_speakers[j])
				output_speakers[j][i] = (input_sample[j] / 255.0f) - 0.5f;
		}
	}
}

void SoundFormatConversion::to_ieee_float(void **output, SpeakerPositionMask output_speakers, const SoundMixingBuffersData &input, unsigned int sample_count)
{
	float * input_speakers[32];
	unsigned int num_output_speakers = find_input_speakers(input, output_speakers, input_speakers);

	unsigned int bytes_per_sample = sizeof(char)*num_output_speakers;
	char *output_pos = reinterpret_cast<char*>(output);
	for (unsigned int i = 0; i < sample_count; i++, output_pos += bytes_per_sample)
	{
		unsigned char *output_sample = reinterpret_cast<unsigned char*>(output_pos);
		for (unsigned int j = 0; j < num_output_speakers; j++)
		{
			if (input_speakers[j])
			{
				float v = (input_speakers[j][i] + 0.5f)*255.0f;
				if (v < 0.0f)
					v = 0.0f;
				else if (v > 255.0f)
					v = 255.0f;
				output_sample[j] = (unsigned char)v;
			}
		}
	}
}

void SoundFormatConversion::to_16bit_le(void **output, SpeakerPositionMask output_speakers, const SoundMixingBuffersData &input, unsigned int sample_count)
{
	float *input_speakers[32];
	unsigned int num_output_speakers = find_input_speakers(input, output_speakers, input_speakers);

	unsigned int bytes_per_sample = sizeof(char)*num_output_speakers;
	char *output_pos = reinterpret_cast<char*>(output);
	for (unsigned int i = 0; i < sample_count; i++, output_pos += bytes_per_sample)
	{
		short *output_sample = reinterpret_cast<short*>(output_pos);
		for (unsigned int j = 0; j < num_output_speakers; j++)
		{
			if (input_speakers[j])
			{
				float v = input_speakers[j][i] * 32768.0f;
				if (v < -32767.0f)
					v = -32767.0f;
				else if (v > 32768.0f)
					v = 32768.0f;
				output_sample[j] = (short)v;
			}
		}
	}
}

void SoundFormatConversion::to_16bit_be(void **output, SpeakerPositionMask output_speakers, const SoundMixingBuffersData &input, unsigned int sample_count)
{
	throw Exception("16 bit big endian samples not implemented");
}

void SoundFormatConversion::to_8bit_signed(void **output, SpeakerPositionMask output_speakers, const SoundMixingBuffersData &input, unsigned int sample_count)
{
	float *input_speakers[32];
	unsigned int num_output_speakers = find_input_speakers(input, output_speakers, input_speakers);

	unsigned int bytes_per_sample = sizeof(char)*num_output_speakers;
	char *output_pos = reinterpret_cast<char*>(output);
	for (unsigned int i = 0; i < sample_count; i++, output_pos += bytes_per_sample)
	{
		unsigned char *output_sample = reinterpret_cast<unsigned char*>(output_pos);
		for (unsigned int j = 0; j < num_output_speakers; j++)
		{
			if (input_speakers[j])
			{
				float v = input_speakers[j][i] * 128.0f;
				if (v < -127.0f)
					v = -127.0f;
				else if (v > 128.0f)
					v = 128.0f;
				output_sample[j] = (char)v;
			}
		}
	}
}

void SoundFormatConversion::to_8bit_unsigned(void **output, SpeakerPositionMask output_speakers, const SoundMixingBuffersData &input, unsigned int sample_count)
{
	float *input_speakers[32];
	unsigned int num_output_speakers = find_input_speakers(input, output_speakers, input_speakers);

	unsigned int bytes_per_sample = sizeof(char)*num_output_speakers;
	char *output_pos = reinterpret_cast<char*>(output);
	for (unsigned int i = 0; i < sample_count; i++, output_pos += bytes_per_sample)
	{
		unsigned char *output_sample = reinterpret_cast<unsigned char*>(output_pos);
		for (unsigned int j = 0; j < num_output_speakers; j++)
		{
			if (input_speakers[j])
			{
				float v = (input_speakers[j][i] + 0.5f)*255.0f;
				if (v < 0.0f)
					v = 0.0f;
				else if (v > 255.0f)
					v = 255.0f;
				output_sample[j] = (unsigned char)v;
			}
		}
	}
}

unsigned int SoundFormatConversion::find_output_speakers(SoundMixingBuffersData &output, SpeakerPositionMask input_speakers, float **output_speakers)
{
	unsigned int num_output_speakers = 0;
	for (unsigned int i = 0; input_speakers >> i; i++)
	{
		if ((input_speakers >> i) & 1)
			output_speakers[num_output_speakers++] = output.channels[i];
	}
	return num_output_speakers;
}

unsigned int SoundFormatConversion::find_input_speakers(const SoundMixingBuffersData &input, SpeakerPositionMask output_speakers, float **input_speakers)
{
	unsigned int num_input_speakers = 0;
	for (unsigned int i = 0; output_speakers >> i; i++)
	{
		if ((output_speakers >> i) & 1)
			input_speakers[num_input_speakers++] = input.channels[i];
	}
	return num_input_speakers;
}

void SoundFormatConversion::clear_unused_speakers(SoundMixingBuffersData &output, SpeakerPositionMask input_speakers, unsigned int sample_count)
{
	for (unsigned int i = 0; input_speakers >> i; i++)
	{
		if (((input_speakers >> i) & 1) == 0 && output.channels[i])
		{
			for (unsigned int j = 0; j < sample_count; j++)
				output.channels[i][j] = 0.0f;
		}
	}
}
