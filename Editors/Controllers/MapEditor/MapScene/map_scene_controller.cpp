
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

void MapSceneController::update_scene(const ScenePtr &scene, const uicore::GraphicContextPtr &gc, const uicore::DisplayWindowPtr &ic, const uicore::Vec2i &mouse_delta)
{
	gametime.update();

	rotation.y = std::fmod(rotation.y + mouse_delta.x * gametime.get_time_elapsed() * mouse_speed_x, 360.0f);
	rotation.x = clamp(rotation.x + mouse_delta.y * gametime.get_time_elapsed() * mouse_speed_y, -90.0f, 90.0f);

	Quaternionf camera_quaternion(rotation.x, rotation.y, rotation.z, angle_degrees, order_YXZ);

	if (view->has_focus())
	{
		Vec3f thrust;
		if (ic->keyboard()->keycode(keycode_a) || ic->keyboard()->keycode(keycode_q))
			thrust.x -= 1.0f;
		if (ic->keyboard()->keycode(keycode_d))
			thrust.x += 1.0f;
		if (ic->keyboard()->keycode(keycode_w) || ic->keyboard()->keycode(keycode_z))
			thrust.z += 1.0f;
		if (ic->keyboard()->keycode(keycode_s))
			thrust.z -= 1.0f;
		if (ic->keyboard()->keycode(keycode_lshift) || ic->keyboard()->keycode(keycode_space))
			thrust.y += 1.0f;
		if (ic->keyboard()->keycode(keycode_lcontrol))
			thrust.y -= 1.0f;

		position += camera_quaternion.rotate_vector(thrust) * gametime.get_time_elapsed() * move_speed;
	}

	scene->camera()->set_position(position);
	scene->camera()->set_orientation(camera_quaternion);

	auto model = MapAppModel::instance();
	if (objects.empty())
	{
		for (const auto &obj_desc : model->desc.objects)
		{
			auto object = std::make_shared<MapSceneObject>();
			objects.push_back(object);

			if (obj_desc.mesh.empty())
				continue;

			auto &model = models[obj_desc.mesh];
			auto &shape = shapes[obj_desc.mesh];
			if (!model)
			{
				try
				{
					auto model_desc = ModelDesc::load(obj_desc.mesh);
					auto fbx = std::make_shared<FBXModel>(model_desc.fbx_filename);
					auto model_data = fbx->convert(model_desc);
					model = SceneModel::create(scene, model_data);
					shape = Physics3DShape::model(model_data);
				}
				catch (...)
				{
					// To do: create dummy placeholder
					continue;
				}
			}

			Quaternionf rotation(obj_desc.up, obj_desc.dir, obj_desc.tilt, angle_degrees, order_YXZ);
			object->scene_object = SceneObject::create(scene, model, obj_desc.position, rotation, Vec3f(obj_desc.scale));

			auto scaled_shape = shape;
			if (obj_desc.scale != 1.0f)
				scaled_shape = Physics3DShape::scale_model(shape, Vec3f(obj_desc.scale));

			object->collision_object = Physics3DObject::rigid_body(collision, scaled_shape, 0.0f, obj_desc.position, rotation);
			object->collision_object->set_data(object.get());
		}
	}

	scene->update(gc, gametime.get_time_elapsed());
}
