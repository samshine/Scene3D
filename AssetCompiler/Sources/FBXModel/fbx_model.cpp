
#include "precomp.h"
#include "AssetCompiler/FBXModel/fbx_model.h"
#include "fbx_model_loader.h"
#include "fbx_model_impl.h"
#include <ClanLib/core.h>

#pragma comment(lib, "libfbxsdk-mt.lib")

namespace clan
{
	FBXModel::FBXModel(const std::string &filename) : impl(std::make_shared<FBXModelImpl>(filename))
	{
	}

	std::shared_ptr<ModelData> FBXModel::convert(const std::vector<FBXAnimation> &animations, const std::vector<FBXAttachmentPoint> &attachment_points, const std::vector<FBXParticleEmitter> &emitters)
	{
		FBXModelLoader loader(impl.get(), animations, attachment_points, emitters);
		return loader.model_data;
	}

	/////////////////////////////////////////////////////////////////////////

	FBXModelImpl::FBXModelImpl(const std::string &filename)
		: base_path(PathHelp::get_fullpath(filename)), manager(nullptr), iosettings(nullptr), scene(nullptr)
	{
		try
		{
			import_scene(filename);
			triangulate_scene();
			//bake_geometric_transforms();
		}
		catch (...)
		{
			if (manager)
				manager->Destroy();
			throw;
		}
	}

	FBXModelImpl::~FBXModelImpl()
	{
		if (manager)
			manager->Destroy();
	}

	void FBXModelImpl::triangulate_scene()
	{
		FbxGeometryConverter converter(manager);
		converter.Triangulate(scene, true);
	}

	void FBXModelImpl::import_scene(const std::string &filename)
	{
		manager = FbxManager::Create();
		if (!manager)
			throw Exception("FbxManager::Create failed");

		iosettings = FbxIOSettings::Create(manager, IOSROOT);
		if (!iosettings)
			throw Exception("FbxIOSettings::Create failed");

		manager->SetIOSettings(iosettings);

		FbxString plugins_directory = FbxGetApplicationDirectory();
		bool result = manager->LoadPluginsDirectory(plugins_directory.Buffer());
		if (!result)
			throw Exception("FbxManager::LoadPluginsDirectory failed");

		scene = FbxScene::Create(manager, "My Scene");
		if (!scene)
			throw Exception("FbxScene::Create failed");


		// Convert to DirectX coordinate system:
		// Note: this line MUST be before importing a scene or something bugs in FBX! (or the documentation is unclear)
		FbxAxisSystem::DirectX.ConvertScene(scene);


		FbxImporter *importer = FbxImporter::Create(manager, "");
		if (!importer)
			throw Exception("FbxImporter::Create failed");

		result = importer->Initialize(filename.c_str(), -1, manager->GetIOSettings());
		if (!result)
			throw Exception(importer->GetStatus().GetErrorString());

		result = importer->Import(scene);
		if (!result && importer->GetStatus().GetCode() == FbxStatus::ePasswordError)
			throw Exception("FBX file is password protected!");

		importer->Destroy();
	}

	void FBXModelImpl::bake_geometric_transforms(FbxNode *node)
	{
		// bake transform components: pre- and post-rotation, rotation and scale pivots and offsets - into the standard transforms

		if (node == nullptr)
		{
			float sampling_rate = 60.0f;
			bake_geometric_transforms(scene->GetRootNode());
			scene->GetRootNode()->ConvertPivotAnimationRecursive(nullptr, FbxNode::eDestinationPivot, sampling_rate);
		}
		else
		{
			// We set up what we want to bake via ConvertPivotAnimationRecursive.
			// When the destination is set to 0, baking will occur.
			// When the destination value is set to the source’s value, the source values will be retained and not baked.

			FbxVector4 zero(0, 0, 0);

			// Activate pivot converting
			node->SetPivotState(FbxNode::eSourcePivot, FbxNode::ePivotActive);
			node->SetPivotState(FbxNode::eDestinationPivot, FbxNode::ePivotActive);

			// We want to set all these to 0 and bake them into the transforms.
			node->SetPostRotation(FbxNode::eDestinationPivot, zero);
			node->SetPreRotation(FbxNode::eDestinationPivot, zero);
			node->SetRotationOffset(FbxNode::eDestinationPivot, zero);
			node->SetScalingOffset(FbxNode::eDestinationPivot, zero);
			node->SetRotationPivot(FbxNode::eDestinationPivot, zero);
			node->SetScalingPivot(FbxNode::eDestinationPivot, zero);
			node->SetRotationOrder(FbxNode::eDestinationPivot, eEulerXYZ);
			node->SetGeometricTranslation(FbxNode::eDestinationPivot, zero);
			node->SetGeometricRotation(FbxNode::eDestinationPivot, zero);
			node->SetGeometricScaling(FbxNode::eDestinationPivot, zero);
			//node->SetGeometricTranslation(FbxNode::eDestinationPivot, node->GetGeometricTranslation(FbxNode::eSourcePivot));
			//node->SetGeometricRotation(FbxNode::eDestinationPivot, node->GetGeometricRotation(FbxNode::eSourcePivot));
			//node->SetGeometricScaling(FbxNode::eDestinationPivot, node->GetGeometricScaling(FbxNode::eSourcePivot));

			// Keep the quaternion interpolation mode
			node->SetQuaternionInterpolation(FbxNode::eDestinationPivot, node->GetQuaternionInterpolation(FbxNode::eSourcePivot));

			for (int i = 0; i < node->GetChildCount(); i++)
			{
				bake_geometric_transforms(node->GetChild(i));
			}
		}
	}
}
