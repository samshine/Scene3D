
#pragma once

#include "AssetCompiler/AssetCompiler/asset_compiler.h"
#include "AssetCompiler/FBXModel/fbx_model.h"
#include "AssetCompiler/FBXModel/fbx_animation.h"
#include "AssetCompiler/FBXModel/fbx_attachment_point.h"
#include "AssetCompiler/FBXModel/fbx_particle_emitter.h"

#if defined(_MSC_VER)
	#if !defined(_DEBUG)
		#pragma comment(lib, "AssetCompiler.lib")
	#else
		#pragma comment(lib, "AssetCompilerd.lib")
	#endif
#endif
