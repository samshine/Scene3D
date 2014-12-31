
#include "precomp.h"
#include "game_scene.h"

using namespace clan;

GameScene::GameScene()
{
}

void GameScene::set_map_model(const std::string &map_model)
{
	map_filename = map_model;
	map_updated = true;
}

void GameScene::set_model_data(std::shared_ptr<ModelData> new_model_data)
{
	model_data = new_model_data;
	model_updated = true;
}

void GameScene::set_attachments(std::vector<SceneModelAttachment> attachments)
{
	model_attachments = attachments;
	model_updated = true;
}

void GameScene::update(Scene &scene, GraphicContext &gc, InputContext &ic, bool has_focus)
{
	gametime.update();
	update_map(scene, gc);
	update_input(ic, has_focus);
	update_model(scene, gc);
	update_camera(scene, gc);
	scene.update(gc, gametime.get_time_elapsed());
}

void GameScene::update_input(InputContext &ic, bool has_focus)
{
	if (!has_focus)
		return;

	float time_elapsed = gametime.get_time_elapsed();

	float dir_speed = 50.0f;
	if (ic.get_keyboard().get_keycode(keycode_left))
	{
		dir = std::fmod(dir - time_elapsed * dir_speed, 360.0f);
	}
	else if (ic.get_keyboard().get_keycode(keycode_right))
	{
		dir = std::fmod(dir + time_elapsed * dir_speed, 360.0f);
	}

	float up_speed = 50.0f;
	if (ic.get_keyboard().get_keycode(keycode_up))
	{
		up = clamp(up - time_elapsed * up_speed, -90.0f, 90.0f);
	}
	else if (ic.get_keyboard().get_keycode(keycode_down))
	{
		up = clamp(up + time_elapsed * up_speed, -90.0f, 90.0f);
	}
}

void GameScene::update_camera(Scene &scene, GraphicContext &gc)
{
	Physics3DSweepTest sweep_test(collision_world);
	Physics3DShape sphere_shape = Physics3DShape::sphere(0.25f);

	Quaternionf camera_orientation(up, dir, tilt, angle_degrees, order_YXZ);

	float zoom_out = 3.0f;
	Vec3f camera_look_pos = /*controller.get_position() +*/ Vec3f(0.0f, 1.8f, 0.0f);
	Vec3f camera_pos = camera_look_pos + camera_orientation.rotate_vector(Vec3f(0.0f, 0.0f, -zoom_out));

	if (sweep_test.test_first_hit(sphere_shape, camera_look_pos, Quaternionf(), camera_pos, Quaternionf()))
	{
		camera_pos = sweep_test.get_hit_position(0);
	}

	SceneCamera camera = scene.get_camera();
	camera.set_orientation(camera_orientation);
	camera.set_position(camera_pos);
}

void GameScene::update_map(Scene &scene, GraphicContext &gc)
{
	if (map_updated)
	{
		map_object = SceneObject();
		map_collision = Physics3DObject();
		map_updated = false;

		try
		{
			FBXModelDesc model_desc = FBXModelDesc::load(map_filename);
			FBXModel fbx_model(model_desc.fbx_filename);
			auto model_data = fbx_model.convert(model_desc);

			map_object = SceneObject(scene, SceneModel(gc, scene, model_data));
			map_collision = Physics3DObject::collision_body(collision_world, Physics3DShape::model(model_data));
		}
		catch (Exception &)
		{
		}
	}
}

void GameScene::update_model(Scene &scene, GraphicContext &gc)
{
	if (model_updated)
	{
		model_object = SceneObject();
		model_updated = false;

		if (model_data)
		{
			model_object = SceneObject(scene, SceneModel(gc, scene, model_data));
		}

		for (auto &attachment : model_attachments)
		{
			try
			{
				FBXModelDesc model_desc = FBXModelDesc::load(attachment.model_name);

				FBXModel fbx_model(model_desc.fbx_filename);
				auto attachment_model_data = fbx_model.convert(model_desc);

				attachment.model = SceneModel(gc, scene, attachment_model_data);
				attachment.object = SceneObject(scene, attachment.model, Vec3f(), Quaternionf(), Vec3f(attachment.model_scale));
			}
			catch (Exception &)
			{
			}
		}
	}

	if (!model_object.is_null())
	{
		model_object.update(gametime.get_time_elapsed());
		for (auto &attachment : model_attachments)
		{
			if (attachment.object.is_null())
				continue;

			Vec3f attach_pos, attach_scale;
			Quaternionf attach_orientation;
			model_object.get_attachment_location(attachment.attachment_name, attach_pos, attach_orientation, attach_scale);
			attachment.object.set_position(attach_pos);
			attachment.object.set_orientation(attach_orientation);
			attachment.object.set_scale(attach_scale * attachment.model_scale);
			attachment.object.update(gametime.get_time_elapsed());
		}
	}
}
