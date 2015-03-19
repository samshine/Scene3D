
#pragma once

#include "AssetCompiler/AssetCompiler/asset_compiler.h"
#include "AssetCompiler/ModelDescription/model_desc.h"
#include "AssetCompiler/ModelDescription/model_desc_animation.h"
#include "AssetCompiler/ModelDescription/model_desc_attachment_point.h"
#include "AssetCompiler/ModelDescription/model_desc_particle_emitter.h"
#include "AssetCompiler/ModelDescription/model_desc_material.h"
#include "AssetCompiler/MapDescription/map_desc.h"
#include "AssetCompiler/FBXModel/fbx_model.h"

#if defined(_MSC_VER)
	#if !defined(_DEBUG)
		#pragma comment(lib, "AssetCompiler.lib")
	#else
		#pragma comment(lib, "AssetCompilerd.lib")
	#endif
#endif
