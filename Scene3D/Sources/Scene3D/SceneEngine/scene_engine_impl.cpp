
#include "precomp.h"
#include "scene_engine_impl.h"
#include "Scene3D/ModelData/model_data.h"
#include "Scene3D/SceneEngine/Model/model.h"
#include "Scene3D/Performance/scope_timer.h"
#include "Scene3D/Scene/scene_impl.h"

using namespace uicore;

SceneEngineImpl::SceneEngineImpl(const GraphicContextPtr &gc, const std::string &shader_path)
	: gc(gc), render(gc, shader_path, this)
{
}

SceneEngineImpl::~SceneEngineImpl()
{
	work_queue.reset(); // Make sure all pending items finish before we shut down
}

void SceneEngineImpl::render_scene(const GraphicContextPtr &gc, SceneImpl *scene)
{
	ScopeTimeFunction();

	render.models_drawn = 0;
	render.instances_drawn = 0;
	render.draw_calls = 0;
	render.triangles_drawn = 0;
	render.scene_visits = 0;

	render.gpu_timer.begin_frame(gc);

	render.setup_pass_buffers(gc);

	if (render.field_of_view != scene->camera()->field_of_view())
		render.field_of_view = scene->camera()->field_of_view();

	Quaternionf inv_orientation = Quaternionf::inverse(scene->camera()->orientation());
	render.world_to_eye = inv_orientation.to_matrix() * Mat4f::translate(-scene->camera()->position());

	for (const auto &pass : render.passes)
	{
		render.gpu_timer.begin_time(gc, pass->name());
		pass->run(gc, scene);
		render.gpu_timer.end_time(gc);
	}

	render.gpu_timer.end_frame(gc);

	render.gpu_results = render.gpu_timer.get_results(gc);

	if (gc->shader_language() == shader_glsl)
		OpenGL::check_error();
}

void SceneEngineImpl::update_scene(const GraphicContextPtr &gc, SceneImpl *scene, float time_elapsed)
{
	process_work_completed();
	update_textures(gc, time_elapsed);
	for (const auto &pass : render.passes)
		pass->update(gc, time_elapsed);
	// To do: update scene object animations here too
}

std::shared_ptr<Model> SceneEngineImpl::get_model(const std::string &model_name)
{
	auto &renderer = models[model_name];
	if (!renderer)
	{
		renderer = std::make_shared<Model>(get_gc(), this, get_model_data(model_name), render.instances_buffer.new_offset_index());
	}
	return renderer;
}

std::shared_ptr<ModelData> SceneEngineImpl::get_model_data(const std::string &name)
{
	return ModelData::load(PathHelp::combine("Resources/Assets", name));
}

void SceneEngineImpl::update_textures(const GraphicContextPtr &gc, float time_elapsed)
{
	//for (size_t i = 0; i < video_textures.size(); i++)
	//	video_textures[i].update(gc, time_elapsed);
}

std::string SceneEngineImpl::to_key(const std::string &material_name, bool linear)
{
	return material_name + (linear ? "__linear" : "__srgb");
}

Resource<TexturePtr> SceneEngineImpl::get_texture(const GraphicContextPtr &gc, const std::string &material_name, bool linear)
{
	std::string key = to_key(material_name, linear);
	Resource<TexturePtr> texture = textures[key];
	if (!texture.get())
	{
		texture.set(get_dummy_texture(gc));
		textures[key] = texture;

		work_queue->queue_item(std::make_shared<CacheLoadTexture>(this, texture, material_name, linear));
		//CacheLoadTexture t(this, texture, material_name, linear);
		//t.process_work();
		//t.work_completed();
	}
	return texture;
}

Texture2DPtr SceneEngineImpl::get_dummy_texture(const GraphicContextPtr &gc)
{
	if (!dummy_texture)
	{
		int width = 64;
		int height = 64;
		int grid_width = 16;
		int grid_height = 16;
		auto image = PixelBuffer::create(width, height, tf_srgb8_alpha8);
		Vec4ub *pixels = image->data<Vec4ub>();
		for (int y = 0; y < height; y++)
		{
			for (int x = 0; x < width; x++)
			{
				int grid_x = x % grid_width;
				int grid_y = y % grid_height;

				Vec4ub color(0,0,0,255);
				if (grid_x == 0 || grid_x + 1 == grid_width || grid_y == 0 || grid_y + 1 == grid_height)
					color = Vec4ub(0x19, 0x70, 0xff, 255);

				pixels[x+y*width] = color;
			}
		}
		dummy_texture = Texture2D::create(gc, image->size(), tf_srgb8_alpha8, 0);
		dummy_texture->set_image(gc, image);
		dummy_texture->set_mag_filter(filter_linear);
		dummy_texture->set_min_filter(filter_linear_mipmap_linear);
		dummy_texture->set_wrap_mode(wrap_repeat, wrap_repeat);
		dummy_texture->set_max_anisotropy(4.0f);
		dummy_texture->generate_mipmap();
	}
	return dummy_texture;
}

/////////////////////////////////////////////////////////////////////////////

CacheLoadTexture::CacheLoadTexture(SceneEngineImpl *cache, Resource<TexturePtr> texture, const std::string &material_name, bool linear)
: cache(cache), texture(texture), material_name(material_name), linear(linear)
{
}

void CacheLoadTexture::process_work()
{
	std::string extension = PathHelp::get_extension(material_name);
	std::string filename = material_name;

	if (!File::exists(filename))
		filename = PathHelp::combine("Resources/Assets/Textures", filename);

	if (Text::equal_caseless(extension, "ctexture"))
	{
		load_ctexture(filename);
	}
	if (Text::equal_caseless(extension, "blp"))
	{
		load_blp_texture(filename);
	}
	else if (Text::equal_caseless(extension, "dds"))
	{
		load_dds_texture(filename);
	}
	else
	{
		load_clanlib_texture(filename);
	}
}

void CacheLoadTexture::work_completed()
{
	if (!video_file.empty())
	{
		//cache->video_textures.push_back(VideoTexture(video_file, texture));
	}
	else if (pixelbuffer_set)
	{
		if (pixelbuffer_set->dimensions() == texture_2d && pixelbuffer_set->max_level() == 0)
		{
			PixelBufferPtr pixelbuffer = pixelbuffer_set->image(0, 0);
			bool has_mipmaps = is_power_of_two(pixelbuffer->width(), pixelbuffer->height());

			// A very rough calculation of much memory we used so far (doesn't take mipmaps or 16 bit textures properly into account)
			cache->memory_used += pixelbuffer->width() * pixelbuffer->height() * 4;

			try
			{
				Texture2DPtr loaded_texture = Texture2D::create(cache->gc, pixelbuffer->width(), pixelbuffer->height(), pixelbuffer->format(), has_mipmaps ? 0 : 1);
				loaded_texture->set_image(cache->gc, pixelbuffer);

				if (is_power_of_two(pixelbuffer->width(), pixelbuffer->height()))
				{
					loaded_texture->set_min_filter(filter_linear_mipmap_linear);
					loaded_texture->set_mag_filter(filter_linear);
					loaded_texture->generate_mipmap();
					loaded_texture->set_max_anisotropy(4.0f);
				}
				else
				{
					loaded_texture->set_min_filter(filter_linear);
					loaded_texture->set_mag_filter(filter_linear);
				}

				loaded_texture->set_wrap_mode(wrap_repeat, wrap_repeat);
				texture.set(loaded_texture);
			}
			catch (const Exception &e)
			{
				ComPtr<ID3D11Debug> debug;
				ComPtr<ID3D11InfoQueue> info_queue;

				ID3D11Device *device = D3DTarget::get_device_handle(cache->gc);

				HRESULT result = device->QueryInterface(__uuidof(ID3D11Debug), (void**)debug.output_variable());
				if (FAILED(result))
					debug.clear(); // No debug info available.  Should this throw an exception instead?
				result = device->QueryInterface(__uuidof(ID3D11InfoQueue), (void**)info_queue.output_variable());
				if (FAILED(result))
					info_queue.clear(); // No debug messages available.

				std::string debug_text;

				if (debug && info_queue)
				{
					// debug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);

					UINT64 message_count = info_queue->GetNumStoredMessagesAllowedByRetrievalFilter();
					std::vector<char> buffer;
					for (UINT64 i = 0; i < message_count; i++)
					{
						SIZE_T length = 0;
						HRESULT result = info_queue->GetMessage(i, 0, &length);
						if (FAILED(result))
							continue;

						buffer.resize(max(buffer.size(), length));
						D3D11_MESSAGE *message = reinterpret_cast<D3D11_MESSAGE*>(buffer.empty() == false ? &buffer[0] : 0);

						result = info_queue->GetMessage(i, message, &length);
						if (FAILED(result))
							continue;

						std::string text;
						if (message->DescriptionByteLength > 0)
							text = std::string(message->pDescription, message->DescriptionByteLength - 1);
						debug_text += string_format("\r\n%4 (id=%1, category=%2, severity=%3)", message->ID, message->Category, message->Severity, text);
					}
					info_queue->ClearStoredMessages();
				}

				throw Exception(string_format("Could not create texture for %1 (%2x%3): %4\r\nApprox memory used: %5 MB, textures loaded: %6%7", material_name, pixelbuffer->width(), pixelbuffer->height(), e.message, cache->memory_used / (double)(1024*1024), cache->textures.size(), debug_text));
			}
		}
		else
		{
			auto loaded_texture = Texture::create(cache->gc, pixelbuffer_set);
			texture.set(loaded_texture);
		}
	}
}

bool CacheLoadTexture::is_power_of_two(int width, int height)
{
	int pot_width = 0, pot_height = 0;
	while ((1 << pot_width) < width)
		pot_width++;
	while ((1 << pot_height) < height)
		pot_height++;
	return (width == (1 << pot_width) && height == (1 << pot_height));
}

void CacheLoadTexture::load_ctexture(const std::string &material_name)
{
	/*try
	{
		DomDocument xml(File(material_name), false);
		std::string base_path = PathHelp::get_fullpath(material_name);
		if (!xml.select_nodes("/ctexture/video").empty())
		{
			video_file = PathHelp::combine(base_path, xml.select_string("/ctexture/video/text()"));
		}
		else
		{
			linear = true; // 3D textures can't use sRGB textures apparently!

			std::string basepath = PathHelp::get_fullpath(material_name);
			std::vector<DomNode> frames = xml.select_nodes("/ctexture/frames/frame");
			std::vector<PixelBufferPtr> images;
			for (size_t i = 0; i < frames.size(); i++)
			{
				std::string filename = frames[i].select_string("filename/text()");
				PixelBufferPtr image = ImageProviderFactory::load(PathHelp::combine(basepath, filename), std::string(), !linear);
				image.flip_vertical();
				image.premultiply_alpha();
				if (image.get_format() == tf_rgba16 && !linear)
					image.premultiply_gamma(2.2f);
				image = image.to_format(linear ? tf_rgba8 : tf_srgb8_alpha8);
				images.push_back(image);
			}

			pixelbuffer_set = PixelBufferSetPtr(texture_3d, linear ? tf_rgba8 : tf_srgb8_alpha8, images.front().get_width(), images.front().get_height(), (int)frames.size());
			for (size_t i = 0; i < images.size(); i++)
			{
				pixelbuffer_set.set_image((int)i, 0, images[i]);
			}
		}
	}
	catch (const Exception &e)
	{
		log_event("Debug", "Unable to load texture %1: %2", material_name, e.message);
	}*/
}

void CacheLoadTexture::load_clanlib_texture(const std::string &material_name)
{
	try
	{
		/*if (material_name.find("poutrerouille90a.png") != std::string::npos)
		{
			int width = 64;
			int height = 64;
			int grid_width = 16;
			int grid_height = 16;
			PixelBufferPtr image(width, height, tf_srgb8_alpha8);
			Vec4ub *pixels = image.get_data<Vec4ub>();
			for (int y = 0; y < height; y++)
			{
				for (int x = 0; x < width; x++)
				{
					int grid_x = x % grid_width;
					int grid_y = y % grid_height;

					Vec4ub color(255, 0, 0, 255);
					if (grid_x == 0 || grid_x + 1 == grid_width || grid_y == 0 || grid_y + 1 == grid_height)
						color = Vec4ub(0, 0, 255, 255);

					pixels[x + y*width] = color;
				}
			}

			pixelbuffer_set = PixelBufferSetPtr(image);
			return;
		}*/

		/*ImageImportDescription import_desc;
		import_desc.set_premultiply_alpha(true);
		import_desc.set_flip_vertical(true);
		import_desc.set_srgb(true);*/
		PixelBufferPtr image = ImageFile::load(material_name, std::string(), !linear);

		//if (image->width() > 1024 || image->height() > 1024)
		//	return; // Limit textures to 1024 for now

		//image->flip_vertical();
		image->premultiply_alpha();

		// Convert to DXT
		/*if (is_power_of_two(image.get_width(), image.get_height()))
		{
			image = image.to_format(tf_rgba8);

			crn_comp_params comp_params;
			comp_params.m_width = image.get_width();
			comp_params.m_height = image.get_height();
			comp_params.m_format = cCRNFmtDXT5;
			comp_params.m_dxt_quality = cCRNDXTQualitySuperFast;
			comp_params.m_file_type = cCRNFileTypeDDS;
			comp_params.m_pImages[0][0] = image.get_data<crn_uint32>();
			if (linear)
				comp_params.set_flag(cCRNCompFlagPerceptual, false);

			crn_mipmap_params mip_params;

			crn_uint32 dds_file_size = 0;
			void *dds_file_data = crn_compress(comp_params, mip_params, dds_file_size);
			if (!dds_file_data)
				throw Exception("Unable to compress image");
			try
			{
				DataBuffer buffer(dds_file_data, dds_file_size);
				IODevice_Memory file(buffer);
				PixelBufferSetPtr set = DDSProvider::load(file);
				int levels = set.get_max_level();
				for (int i = 0; i < levels; i++)
				{
					loaded_levels.push_back(TextureLevel(set.get_image(0, i)));
				}
			}
			catch (...)
			{
				crn_free_block(dds_file_data);
				throw;
			}
			crn_free_block(dds_file_data);
		}
		else*/
		{
			// Convert sRGB 16 bit image to linear:
			if (image->format() == tf_rgba16 && !linear)
				image->premultiply_gamma(2.2f);

			pixelbuffer_set = PixelBufferSet::create(image);
		}
	}
	catch (const Exception &)
	{
		//log_event("Debug", "Unable to load texture %1: %2", material_name, e.message);
	}
}

void CacheLoadTexture::load_dds_texture(const std::string &material_name)
{
	try
	{
		pixelbuffer_set = DDSFormat::load(material_name);
	}
	catch (const Exception &)
	{
		//log_event("Debug", "Unable to load texture %1: %2", material_name, e.message);
	}
}

void CacheLoadTexture::load_blp_texture(const std::string &material_name)
{
/*
	try
	{
		File file(material_name);
		char blp[4];
		file.read(blp, 4);
		if (std::string(blp, 4) != "BLP2")
			throw Exception("Not a BLP2 file");
		unsigned int type = file.read_uint32();
		unsigned int compression = file.read_uint8();
		unsigned int alpha_bit_depth = file.read_uint8();
		unsigned int alpha_encoding = file.read_uint8();
		unsigned int has_mipmaps = file.read_uint8();
		unsigned int width = file.read_uint32();
		unsigned int height = file.read_uint32();
		unsigned int mipmap_offsets[16];
		unsigned int mipmap_sizes[16];
		unsigned int palette_argb[256];
		file.read(mipmap_offsets, sizeof(unsigned int) * 16);
		file.read(mipmap_sizes, sizeof(unsigned int) * 16);
		file.read(palette_argb, 256 * sizeof(unsigned int));
		if (type == 0)
		{
			PixelBufferPtr image(1, 1, tf_rgba8);
			image.get_data_uint32()[0] = 0xff0000ff;

			pixelbuffer_set = PixelBufferSetPtr(image);
			//throw Exception("Unsupported JPEG compression type");
		}
		else if (type == 1)
		{
			if (compression == 1) // uncompressed 256 palette
			{
				for (int level = 0; mipmap_offsets[level] > 0; level++)
				{
					DataBuffer image_data(width * height);
					file.seek(mipmap_offsets[level]);
					file.read(image_data.get_data(), image_data.get_size());

					PixelBufferPtr image(width, height, tf_srgb8_alpha8);
					PixelBufferPtrLock4ub output(image);

					for (unsigned int y = 0; y < height; y++)
					{
						unsigned char *input_line = reinterpret_cast<unsigned char*>(image_data.get_data() + width * y);
						Vec4ub *output_line = output.get_row(y);
						for (unsigned int x = 0; x < width; x++)
						{
							unsigned char alpha = (palette_argb[input_line[x]] >> 24) & 0xff;
							unsigned char red = (palette_argb[input_line[x]] >> 16) & 0xff;
							unsigned char green = (palette_argb[input_line[x]] >> 8) & 0xff;
							unsigned char blue = palette_argb[input_line[x]] & 0xff;
							output_line[x] = Vec4ub(red, green, blue, alpha);
						}
					}

					loaded_levels.push_back(TextureLevel(image));

					if (width == 1 && height == 1 || has_mipmaps == 0)
						break;

					if (width > 1)
						width /= 2;
					if (height > 1)
						height /= 2;
				}
			}
			else if (compression == 2) // DXTC
			{
				TextureFormat format;
				int s3tc_bytes_per_block = 8;
				if (alpha_bit_depth == 0)
				{
					format = cl_compressed_srgb_s3tc_dxt1;
				}
				else if (alpha_bit_depth == 1)
				{
					format = cl_compressed_srgb_alpha_s3tc_dxt1;
				}
				else if (alpha_bit_depth == 4 && alpha_encoding == 1)
				{
					format = cl_compressed_srgb_alpha_s3tc_dxt3;
					s3tc_bytes_per_block = 16;
				}
				else if (alpha_bit_depth == 8 && alpha_encoding == 1)
				{
					format = cl_compressed_srgb_alpha_s3tc_dxt3;
					s3tc_bytes_per_block = 16;
				}
				else if (alpha_bit_depth == 8 && alpha_encoding == 7)
				{
					format = cl_compressed_srgb_alpha_s3tc_dxt5;
					s3tc_bytes_per_block = 16;
				}
				else
				{
					throw Exception("Unsupported compression type");
				}

				unsigned int pot_width = 0;
				unsigned int pot_height = 0;
				while ((((unsigned int)1) << pot_width) < width)
					pot_width++;
				while ((((unsigned int)1) << pot_height) < height)
					pot_height++;
				unsigned int levels = has_mipmaps ? min(max(pot_width, pot_height) + 1, (unsigned int)16) : (unsigned int)1;

				for (unsigned int level = 0; level < levels; level++)
				{
					if (mipmap_offsets[level] == 0 || mipmap_sizes[level] == 0)
						throw Exception("Broken mipmap count");

					unsigned int level_width = max(width >> level, (unsigned int)1);
					unsigned int level_height = max(height >> level, (unsigned int)1);

					unsigned int s3tc_width = (level_width + 3) / 4;
					unsigned int s3tc_height = (level_height + 3) / 4;
					unsigned int s3tc_size = s3tc_width * s3tc_height * s3tc_bytes_per_block;

					DataBuffer image(s3tc_size);
					file.seek(mipmap_offsets[level]);
					file.read(image.get_data(), image.get_size());

					// For some reason the mipmap sizes specified in the file appear to be incorrect.
					// I.e. a 8x2 DXT1 image only has 8 bytes of data, while it should have 16 bytes.
					// This code repeats the data available until we get a full image.
					unsigned int mipmap_size = min(s3tc_size, mipmap_sizes[level]);
					unsigned char *image_data = reinterpret_cast<unsigned char *>(image.get_data());
					for (unsigned int i = mipmap_size; i < s3tc_size; i++)
						image_data[i] = image_data[i % mipmap_size];

					if (format == cl_compressed_srgb_alpha_s3tc_dxt5)
					{
						// Some of the DXT5 textures are transparent when they shouldn't be.  Seems there are some flags
						// where the textures are used that tell whether the alpha bits are to be used or not. Since this
						// isn't supported currently, we force all DXT5 textures to be opaque for now.
						for (unsigned int block = 0; block < s3tc_size / 16; block++)
						{
							// Force alpha to 1.0:
							for (int j = 0; j < 8; j++)
								image_data[block * 16 + j] = 255;
						}
					}

					loaded_levels.push_back(TextureLevel(format, level_width, level_height, image));
				}
			}
			else
			{
				throw Exception("Unknown compression type");
			}
		}
		else
		{
			throw Exception("Unknown BLP2 type");
		}
	}
	catch (const Exception &)
	{
		loaded_levels.clear();
		PixelBufferPtr image(1, 1, tf_rgba8);
		image.get_data_uint32()[0] = 0xff0000ff;
		loaded_levels.push_back(TextureLevel(image));
	}
*/
}
