
#include "precomp.h"
#include "Sound/SoundProviders/soundprovider_wave.h"
#include "soundprovider_wave_impl.h"
#include "soundprovider_wave_session.h"

using namespace uicore;

SoundProvider_Wave::SoundProvider_Wave(const std::string &filename, bool stream) : impl(std::make_shared<SoundProvider_Wave_Impl>())
{
	auto source = File::open_existing(filename);
	impl->load(*source);
}

SoundProvider_Wave::SoundProvider_Wave(IODevice &file, bool stream) : impl(std::make_shared<SoundProvider_Wave_Impl>())
{
	impl->load(file);
}

SoundProvider_Wave::~SoundProvider_Wave()
{
}

SoundProvider_Session *SoundProvider_Wave::begin_session()
{
	return new SoundProvider_Wave_Session(*this);
}

void SoundProvider_Wave::end_session(SoundProvider_Session *session)
{
	delete session;
}

void SoundProvider_Wave_Impl::load(IODevice &source)
{
	char chunk_id[4];
	source.read(chunk_id, 4);
	if (memcmp(chunk_id, "RIFF", 4))
		throw Exception("Expected RIFF header!");
	uint32_t chunk_size = source.read_uint32();

	char format_id[4];
	source.read(format_id, 4);
	if (memcmp(format_id, "WAVE", 4))
		throw Exception("Expected WAVE header!");

	uint32_t subchunk_pos = source.position();
	uint32_t subchunk1_size = find_subchunk("fmt ", source, subchunk_pos, chunk_size);

	uint16_t audio_format = source.read_uint16();
	num_channels = source.read_uint16();
	frequency = source.read_uint32();
	uint32_t byte_rate = source.read_uint32();
	uint16_t block_align = source.read_uint16();
	uint16_t bits_per_sample = source.read_uint16();

	if (bits_per_sample == 16)
		format = sf_16bit_signed;
	else if (bits_per_sample == 8)
		format = sf_8bit_unsigned;
	else
		throw Exception("Unsupported wave sample format");

	uint32_t subchunk2_size = find_subchunk("data", source, subchunk_pos, chunk_size);

	data = new char[subchunk2_size];
	source.read(data, subchunk2_size);

	num_samples = subchunk2_size / block_align;
}

unsigned int SoundProvider_Wave_Impl::find_subchunk(const char *chunk, IODevice &source, unsigned int file_offset, unsigned int max_offset)
{
	char subchunk1_id[4];

	max_offset -= 8;	// Each subchunk must contains at least name and size
	while (file_offset < max_offset)
	{
		source.seek(file_offset);
		source.read(subchunk1_id, 4);
		uint32_t subchunk1_size = source.read_uint32();
		if (!memcmp(subchunk1_id, chunk, 4))
		{
			// Found chunk
			return subchunk1_size;
		}
		file_offset += subchunk1_size + 8;
	}

	throw Exception("Block not found!");
}
