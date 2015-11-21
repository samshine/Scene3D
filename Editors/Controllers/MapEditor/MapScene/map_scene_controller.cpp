
#include "precomp.h"
#include "map_scene_controller.h"
#include "Model/MapEditor/map_app_model.h"
#include "Views/Scene/scene_view.h"

using namespace uicore;

MapSceneController::MapSceneController()
{
	set_root_view(view);

	slots.connect(MapAppModel::instance()->sig_map_updated, this, &MapSceneController::map_updated);
	slots.connect(view->sig_update_scene, this, &MapSceneController::update_scene);
}

void MapSceneController::map_updated()
{
	objects.clear();
}

void MapSceneController::update_scene(const ScenePtr &scene, const uicore::GraphicContextPtr &gc, const uicore::DisplayWindowPtr &ic, const uicore::Vec2i &)
{
	auto model = MapAppModel::instance();
	if (objects.empty())
	{
		for (const auto &obj_desc : model->desc.objects)
		{
			auto &model = models[obj_desc.mesh];
			if (!model)
			{
				auto model_desc = ModelDesc::load(obj_desc.mesh);
				auto fbx = std::make_shared<FBXModel>(model_desc.fbx_filename);
				auto model_data = fbx->convert(model_desc);
				model = SceneModel::create(scene, model_data);
			}

			Quaternionf rotation(obj_desc.up, obj_desc.dir, obj_desc.tilt, order_YXZ);
			auto object = SceneObject::create(scene, model, obj_desc.position, rotation, Vec3f(obj_desc.scale));
			objects.push_back(object);
		}
	}
}
