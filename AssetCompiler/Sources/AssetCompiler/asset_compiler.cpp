
#include "precomp.h"
#include "AssetCompiler/AssetCompiler/asset_compiler.h"
#include "AssetCompiler/ModelDescription/model_desc.h"
#include "AssetCompiler/MapDescription/map_desc.h"
#include "AssetCompiler/FBXModel/fbx_model.h"
#include "TextureBuilder/texture_builder.h"
#include "asset_compiler_impl.h"

using namespace uicore;

AssetCompiler::AssetCompiler(const std::string &asset_directory, const std::string &build_directory, const std::function<void(const CompilerMessage&)> &log) : impl(std::make_shared<AssetCompilerImpl>())
{
	impl->asset_directory = asset_directory;
	impl->build_directory = build_directory;
	impl->log_message = log;
}

void AssetCompiler::cancel()
{
	impl->cancel();
}

void AssetCompiler::clean()
{
	impl->clean();
}

void AssetCompiler::build()
{
	impl->build();
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
					auto path = PathHelp::get_fullpath(filename);
					auto name = PathHelp::get_filename(filename);
					auto ext = PathHelp::get_extension(filename);

					auto output_path = PathHelp::combine(build_directory, PathHelp::make_relative(asset_directory, path));

					if (Text::equal_caseless(ext, "mapdesc"))
					{
						log_message({ CompilerMessageType::info, name });

						MapDesc desc = MapDesc::load(filename);
						std::shared_ptr<MapData> map_data = desc.convert();

						Directory::create(output_path, true);
						MapData::save(File::create_always(PathHelp::combine(output_path, name + ".cmap")), map_data);
					}
					else if (Text::equal_caseless(ext, "modeldesc"))
					{
						log_message({ CompilerMessageType::info, name });

						ModelDesc desc = ModelDesc::load(filename);

						FBXModel model(desc.fbx_filename);
						std::shared_ptr<ModelData> model_data = model.convert(desc);

						auto texture_output_path = PathHelp::combine(build_directory, "Textures");
						Directory::create(texture_output_path, true);
						TextureBuilder::build(model_data, texture_output_path);

						Directory::create(output_path, true);
						ModelData::save(File::create_always(PathHelp::combine(output_path, name + ".cmodel")), model_data);
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
