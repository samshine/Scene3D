
#include "precomp.h"
#include "AssetCompiler/AssetCompiler/asset_compiler.h"
#include "AssetCompiler/FBXModel/fbx_model.h"
#include "AssetCompiler/FBXModel/fbx_animation.h"
#include "AssetCompiler/FBXModel/fbx_attachment_point.h"
#include "AssetCompiler/FBXModel/fbx_particle_emitter.h"
#include "asset_compiler_impl.h"

namespace clan
{
	AssetCompiler::AssetCompiler() : impl(std::make_shared<AssetCompilerImpl>())
	{
	}

	void AssetCompiler::compile(const std::string &filename, const std::function<void(const CompilerMessage&)> &output)
	{
		try
		{
			std::string output_filename = PathHelp::combine(PathHelp::get_fullpath(filename), PathHelp::get_filename(filename) + ".cmodel");

			FBXModel model(filename);
			std::vector<FBXAnimation> animations;
			std::vector<FBXAttachmentPoint> attachments;
			std::vector<FBXParticleEmitter> emitters;

			animations.push_back(FBXAnimation());
			animations[0].name = "default";

			std::shared_ptr<ModelData> model_data = model.convert(animations, attachments, emitters);

			File file(output_filename, File::create_always, File::access_read_write);
			ModelData::save(file, model_data);
		}
		catch (Exception &e)
		{
			output(CompilerMessage(CompilerMessageType::error, e.message));
		}
	}
}
