
#include "precomp.h"
#include "AssetCompiler/asset_compiler.h"
#include "asset_compiler_impl.h"

namespace clan
{
	AssetCompiler::AssetCompiler() : impl(std::make_shared<AssetCompilerImpl>())
	{
	}

	void AssetCompiler::compile(const std::string &filename, const std::function<void(const CompilerMessage&)> &output)
	{
	}
}
