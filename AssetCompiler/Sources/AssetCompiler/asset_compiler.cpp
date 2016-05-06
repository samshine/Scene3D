
#include "precomp.h"
#include "AssetCompiler/ModelDescription/model_desc.h"
#include "AssetCompiler/MapDescription/map_desc.h"
#include "AssetCompiler/FBXModel/fbx_model.h"
#include "TextureBuilder/texture_builder.h"
#include "Raytracer/raytracer.h"
#include "asset_compiler_impl.h"

using namespace uicore;

std::shared_ptr<AssetCompiler> AssetCompiler::create(const std::string &asset_directory, const std::string &build_directory, const std::function<void(const CompilerMessage&)> &log)
{
	auto impl = std::make_shared<AssetCompilerImpl>();
	impl->asset_directory = asset_directory;
	impl->build_directory = build_directory;
	impl->log_message = log;
	return impl;
}

void AssetCompilerImpl::build()
{
	execute([=]()
	{
		try
		{
			log_message({ CompilerMessageType::info, "Build started.." });

			std::function<void(std::string)> scan_dir;
			scan_dir = [&](std::string path)
			{
				for (const auto &filename : Directory::files(path))
				{
					auto path = FilePath::basepath(filename);
					auto name = FilePath::filename_without_extension(filename);
					auto ext = FilePath::extension(filename);

					auto friendly_name = FilePath::make_relative(asset_directory, filename);

					auto output_path = FilePath::combine(build_directory, FilePath::make_relative(asset_directory, path));

					if (Text::equal_caseless(ext, "mapdesc"))
					{
						log_message({ CompilerMessageType::info, friendly_name });

						MapDesc desc = MapDesc::load(filename);
						std::shared_ptr<MapData> map_data = desc.convert(asset_directory);

						/*if (name == "Liandri")
						{
							RayTracer raytracer(desc);
							raytracer.generate_diffuse_gi(FilePath::combine(output_path, name + ".diffusegi"));
						}*/

						Directory::create(output_path, true);
						MapData::save(File::create_always(FilePath::combine(output_path, name + ".cmap")), map_data);
					}
					else if (Text::equal_caseless(ext, "modeldesc"))
					{
						log_message({ CompilerMessageType::info, friendly_name });

						ModelDesc desc = ModelDesc::load(filename);

						auto model = FBXModel::load(desc.fbx_filename);
						std::shared_ptr<ModelData> model_data = model->convert(desc);

						auto texture_output_path = FilePath::combine(build_directory, "Textures");
						Directory::create(texture_output_path, true);
						TextureBuilder::build(model_data, texture_output_path);

						Directory::create(output_path, true);
						ModelData::save(File::create_always(FilePath::combine(output_path, name + ".cmodel")), model_data);
					}
				}

				for (const auto &dir : Directory::directories(path))
					scan_dir(dir);
			};

			scan_dir(asset_directory);

			log_message({ CompilerMessageType::info, "Build finished." });
		}
		catch (const std::exception &e)
		{
			log_message({ CompilerMessageType::error, "Internal compiler error: " + (std::string)e.what() });
		}
	});
}

void AssetCompilerImpl::clean()
{
	execute([=]()
	{
		log_message({ CompilerMessageType::info, "Clean started.." });
		log_message({ CompilerMessageType::info, "Clean finished." });
	});
}
