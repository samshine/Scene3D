
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
	impl->log = log;
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

#if 0
	try
	{
		std::string filetype = PathHelp::get_extension(filename);
		std::string base_path = PathHelp::get_fullpath(filename);
		std::string output_filename = PathHelp::combine(base_path, PathHelp::get_basename(filename) + ".cmodel");

		if (Text::equal_caseless(filetype, "modeldesc"))
		{
			ModelDesc desc = ModelDesc::load(filename);

			FBXModel model(desc.fbx_filename);
			std::shared_ptr<ModelData> model_data = model.convert(desc);

			TextureBuilder::build(model_data, base_path);

			ModelData::save(File::create_always(output_filename), model_data);
		}
		/*else if (Text::equal_caseless(filetype, "mapdesc"))
		{
			MapDesc desc = MapDesc::load(filename);

			FBXModel model(desc.fbx_filename);
			std::shared_ptr<ModelData> model_data = model.convert(desc);

			TextureBuilder::build(model_data, base_path);

			ModelData::save(File::create_always(output_filename), model_data);
		}*/
		else
		{
			throw Exception("Unknown file type");
		}
	}
	catch (Exception &e)
	{
		output(CompilerMessage(CompilerMessageType::error, e.message));
	}
#endif
}

void AssetCompilerImpl::build()
{
	execute([=]()
	{
		try
		{
			log({ CompilerMessageType::info, "Build started.." });



			log({ CompilerMessageType::info, "Build finished." });
		}
		catch (const std::exception &e)
		{
			log({ CompilerMessageType::error, "Internal compiler error: " + (std::string)e.what() });
		}
	});
}

void AssetCompilerImpl::clean()
{
	execute([=]()
	{
		log({ CompilerMessageType::info, "Clean started.." });
		log({ CompilerMessageType::info, "Clean finished." });
	});
}

/*
// Convert old cmodel files to newer format:

std::function<void(std::string)> scan_dir;

scan_dir = [&](std::string path)
{
	DirectoryScanner dir;
	if (dir.scan(path))
	{
		while (dir.next())
		{
			if (dir.get_name() == "." || dir.get_name() == "..")
				continue;

			if (dir.is_directory())
			{
				scan_dir(dir.get_pathname());
			}
			else if (StringHelp::compare(PathHelp::get_extension(dir.get_name()), "cmodel", true) == 0)
			{
				auto model_data = ModelData::load(dir.get_pathname());
				for (auto &texture : model_data->textures)
				{
					if (!FileHelp::file_exists(texture.name))
						texture.name = PathHelp::make_absolute(path, texture.name);
				}
				TextureBuilder::build(model_data, "C:\\Development\\Workspaces\\Scene3D\\ShooterGame\\Resources\\Assets\\Textures");

				File file(dir.get_pathname(), File::create_always, File::access_read_write);
				ModelData::save(file, model_data);
			}
		}
	}

};
scan_dir("C:\\Development\\Workspaces\\Scene3D\\ShooterGame\\Resources\\Assets");
return;
*/
