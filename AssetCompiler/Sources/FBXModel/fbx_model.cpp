
#include "precomp.h"
#include "AssetCompiler/ModelDescription/model_desc.h"
#include "AssetCompiler/MapDescription/map_desc.h"
#include "Lightmap/lightmap_uv.h"
#include "Lightmap/lightmap_texture.h"
#include "fbx_model_loader.h"
#include "fbx_model_impl.h"

#pragma comment(lib, "libfbxsdk-mt.lib")

using namespace uicore;

std::shared_ptr<FBXModel> FBXModel::load(const std::string &filename)
{
	return std::make_shared<FBXModelImpl>(filename);
}

/////////////////////////////////////////////////////////////////////////

FBXModelImpl::FBXModelImpl(const std::string &filename)
	: base_path(PathHelp::get_fullpath(filename)), manager(nullptr), iosettings(nullptr), scene(nullptr)
{
	try
	{
		import_scene(filename);
		scale_scene();
		triangulate_scene();
		//bake_geometric_transforms();
		inspect_node(scene->GetRootNode());
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

const std::vector<std::string> &FBXModelImpl::material_names() const
{
	return _material_names;
}

const std::vector<std::string> &FBXModelImpl::bone_names() const
{
	return _bone_names;
}

const std::vector<std::string> &FBXModelImpl::light_names() const
{
	return _light_names;
}

const std::vector<std::string> &FBXModelImpl::camera_names() const
{
	return _camera_names;
}

std::shared_ptr<ModelData> FBXModelImpl::convert(const ModelDesc &desc)
{
	FBXModelLoader loader(this, desc);
	return loader.model_data;
}
/*
std::shared_ptr<ModelData> FBXModel::convert(const MapDesc &desc, bool bake_light)
{
	ModelDesc model_desc;
	model_desc.fbx_filename = desc.fbx_filename;
	model_desc.materials = desc.materials;
	model_desc.emitters = desc.emitters;

	FBXModelLoader loader(impl.get(), model_desc);
	std::shared_ptr<ModelData> data = loader.model_data;
	if (bake_light)
	{
		LightmapUV lightmap_uv;
		lightmap_uv.generate(data);

		LightmapTexture lightmap_texture;
		lightmap_texture.generate(data);

		data->lights.clear();
	}
	return data;
}
*/

void FBXModelImpl::inspect_node(FbxNode *node)
{
	FbxNodeAttribute *node_attribute = node->GetNodeAttribute();
	if (node_attribute)
	{
		FbxNodeAttribute::EType node_type = node_attribute->GetAttributeType();
		switch (node_type)
		{
		case FbxNodeAttribute::eMesh:
			inspect_mesh(node);
			break;

		case FbxNodeAttribute::eCamera:
			inspect_camera(node);
			break;

		case FbxNodeAttribute::eLight:
			inspect_light(node);

		case FbxNodeAttribute::eMarker:
		case FbxNodeAttribute::eSkeleton:
		case FbxNodeAttribute::eNurbs:
		case FbxNodeAttribute::ePatch:
		case FbxNodeAttribute::eLODGroup:
			break;
		}
	}

	for (int i = 0; i < node->GetChildCount(); i++)
	{
		inspect_node(node->GetChild(i));
	}
}

void FBXModelImpl::inspect_mesh(FbxNode *node)
{
	FbxMesh *mesh = static_cast<FbxMesh*>(node->GetNodeAttribute());

	for (int material_index = 0; material_index < node->GetMaterialCount(); material_index++)
	{
		FbxSurfaceMaterial *material = node->GetMaterial(material_index);

		std::string name = material->GetName();
		if (std::find(_material_names.begin(), _material_names.end(), name) == _material_names.end())
			_material_names.push_back(name);
	}

	inspect_skins(node, mesh);
}

void FBXModelImpl::inspect_skins(FbxNode *node, FbxMesh *mesh)
{
	int num_skins = mesh->GetDeformerCount(FbxDeformer::eSkin);
	for (int skin_index = 0; skin_index < num_skins; skin_index++)
	{
		FbxSkin *skin = static_cast<FbxSkin *>(mesh->GetDeformer(skin_index, FbxDeformer::eSkin));

		FbxSkin::EType skinning_type = skin->GetSkinningType();
		if (skinning_type != FbxSkin::eLinear && skinning_type != FbxSkin::eRigid)
			continue;

		int num_clusters = skin->GetClusterCount();
		for (int cluster_index = 0; cluster_index < num_clusters; cluster_index++)
		{
			FbxCluster *cluster = skin->GetCluster(cluster_index);
			FbxNode *link = cluster->GetLink();
			if (link)
			{
				std::string name = link->GetName();
				if (std::find(_bone_names.begin(), _bone_names.end(), name) == _bone_names.end())
					_bone_names.push_back(name);
			}
		}
	}
}

void FBXModelImpl::inspect_camera(FbxNode *node)
{
	// FbxCamera *camera = static_cast<FbxCamera*>(node->GetNodeAttribute());

	std::string name = node->GetName();
	if (std::find(_camera_names.begin(), _camera_names.end(), name) == _camera_names.end())
		_camera_names.push_back(name);
}

void FBXModelImpl::inspect_light(FbxNode *node)
{
	// FbxLight *light = static_cast<FbxLight*>(node->GetNodeAttribute());

	std::string name = node->GetName();
	if (std::find(_light_names.begin(), _light_names.end(), name) == _light_names.end())
		_light_names.push_back(name);
}

void FBXModelImpl::triangulate_scene()
{
	FbxGeometryConverter converter(manager);
	converter.Triangulate(scene, true);
}

void FBXModelImpl::scale_scene()
{
	/*
	const FbxSystemUnit::ConversionOptions conversionOptions = {
		false, // mConvertRrsNodes
		true, // mConvertAllLimits
		true, // mConvertClusters
		true, // mConvertLightIntensity
		true, // mConvertPhotometricLProperties
		true  // mConvertCameraClipPlanes
	};
	*/
	FbxSystemUnit::m.ConvertScene(scene, FbxSystemUnit::DefaultConversionOptions);
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
