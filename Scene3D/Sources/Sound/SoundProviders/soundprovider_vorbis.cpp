
#include "precomp.h"
#include "Sound/SoundProviders/soundprovider_vorbis.h"
#include "soundprovider_vorbis_impl.h"
#include "soundprovider_vorbis_session.h"

using namespace uicore;

SoundProvider_Vorbis::SoundProvider_Vorbis(const std::string &filename, const FileSystem &fs, bool stream) : impl(std::make_shared<SoundProvider_Vorbis_Impl>())
{
	IODevice input = fs.open_file(filename, File::open_existing, File::access_read, File::share_all);
	impl->load(input);
}

SoundProvider_Vorbis::SoundProvider_Vorbis(const std::string &fullname, bool stream) : impl(std::make_shared<SoundProvider_Vorbis_Impl>())
{
	std::string path = PathHelp::get_fullpath(fullname, PathHelp::path_type_file);
	std::string filename = PathHelp::get_filename(fullname, PathHelp::path_type_file);
	FileSystem vfs(path);
	IODevice input = vfs.open_file(filename, File::open_existing, File::access_read, File::share_all);
	impl->load(input);
}

SoundProvider_Vorbis::SoundProvider_Vorbis(IODevice &file, bool stream) : impl(std::make_shared<SoundProvider_Vorbis_Impl>())
{
	impl->load(file);
}

SoundProvider_Vorbis::~SoundProvider_Vorbis()
{
}

SoundProvider_Session *SoundProvider_Vorbis::begin_session()
{
	return new SoundProvider_Vorbis_Session(*this);
}

void SoundProvider_Vorbis::end_session(SoundProvider_Session *session)
{
	delete session;
}

void SoundProvider_Vorbis_Impl::load(IODevice &input)
{
	int size = input.get_size();
	buffer = DataBuffer(size);
	int bytes_read = input.read(buffer.get_data(), buffer.get_size());
	buffer.set_size(bytes_read);
}
