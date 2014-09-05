
#include "precomp.h"
#include "cmodel_format.h"
#include "triangle_mesh_shape.h"
#include "sphere_shape.h"
#include "test_scene_cache.h"
#include <algorithm>

int main(int, char **)
{
	SetupCore setup_core;
	SetupDisplay setup_display;
	SetupD3D setup_d3d;

	std::shared_ptr<ModelData> model_data = CModelFormat::load("Dreary/dreary1.cmodel");

	TriangleMeshShape shape(&model_data->meshes.front().vertices[0], model_data->meshes.front().vertices.size(), &model_data->meshes.front().elements[0], model_data->meshes.front().elements.size());
	SphereShape sphere(Vec3f(0.0f, 0.0f, 0.0f), 100.0f);

	Console::write_line("min depth = %1, max depth = %2, average depth = %3, balanced depth = %4", shape.get_min_depth(), shape.get_max_depth(), shape.get_average_depth(), shape.get_balanced_depth());

	{
		ubyte64 start = System::get_microseconds();
		int iterations = 1000000;
		int hit_count = 0;
		for (int i = 0; i < iterations; i++)
		{
			if (TriangleMeshShape::find_any_hit(&shape, &sphere))
				hit_count++;
		}
		ubyte64 end = System::get_microseconds();

		Console::write_line("Overlap hit count = %1, time = %2 microseconds", hit_count, (end - start) / (double)iterations);
	}

	{
		ubyte64 start = System::get_microseconds();
		int iterations = 1000000;
		int hit_count = 0;

		Vec3f sweep_from(500.0f, 100.0f, 50.0f);
		Vec3f sweep_to = sweep_from + Vec3f(0.0f, 0.0f, 250.0f);

		SphereShape sphere(sweep_from, 5.0f);
		for (int i = 0; i < iterations; i++)
		{
			float t = TriangleMeshShape::sweep(&shape, &sphere, sweep_to);
			if (t < 1.0f)
				hit_count++;
		}
		ubyte64 end = System::get_microseconds();

		Console::write_line("Sweep hit count = %1, time = %2 microseconds", hit_count, (end - start) / (double)iterations);
	}

	return 0;
}

/*
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	SetupCore setup_core;
	SetupDisplay setup_display;
	SetupD3D setup_d3d;

	DisplayWindow window("Scene3D Example", 1600, 900, false, true);

	GraphicContext gc = window.get_gc();

	bool exit = false;
	Slot slot_close = window.sig_window_close().connect_functor([&exit]() { exit = true; });

	ResourceManager resources;
	SceneCache::set(resources, std::shared_ptr<SceneCache>(new TestSceneCache()));

	Scene scene(gc, resources, "../ClanLib-3.0/Resources/Scene3D");
	scene.set_cull_oct_tree(10000.0f);

	SceneCamera camera(scene);
	scene.set_camera(camera);

	SceneLight light(scene);
	light.set_attenuation_end(2000.0f);

	std::shared_ptr<ModelData> model_data = CModelFormat::load("Dreary/dreary1.cmodel");

	SceneModel model_shape(gc, scene, model_data);
	SceneObject model_object(scene, model_shape);

	SceneObject model_box(scene, SceneModel(gc, scene, "box"));

	TriangleMeshShape shape(&model_data->meshes.front().vertices[0], model_data->meshes.front().vertices.size(), &model_data->meshes.front().elements[0], model_data->meshes.front().elements.size());

	GameTime game_time;

	float up = 20.0f;
	float tilt = 0.0f;
	float dir = 0.0f;

	Vec3f our_pos(500.0f, 100.0f, 50.0f);

	while (!exit)
	{
		KeepAlive::process();
		game_time.update();

		// Update camera orientation:

		float dir_speed = 50.0f;
		if (window.get_ic().get_keyboard().get_keycode(keycode_left))
		{
			dir = std::fmod(dir - game_time.get_time_elapsed() * dir_speed, 360.0f);
		}
		else if (window.get_ic().get_keyboard().get_keycode(keycode_right))
		{
			dir = std::fmod(dir + game_time.get_time_elapsed() * dir_speed, 360.0f);
		}

		float up_speed = 50.0f;
		if (window.get_ic().get_keyboard().get_keycode(keycode_down))
		{
			up = clamp(up - game_time.get_time_elapsed() * up_speed, -90.0f, 90.0f);
		}
		else if (window.get_ic().get_keyboard().get_keycode(keycode_up))
		{
			up = clamp(up + game_time.get_time_elapsed() * up_speed, -90.0f, 90.0f);
		}

		Quaternionf camera_orientation(up, dir, tilt, angle_degrees, order_YXZ);

		// Move camera

		Vec3f move_velocity;
		if (window.get_ic().get_keyboard().get_keycode(keycode_w))
		{
			our_pos += camera_orientation.rotate_vector(Vec3f(0.0f, 0.0f, 100.0f)) * game_time.get_time_elapsed();
		}
		else if (window.get_ic().get_keyboard().get_keycode(keycode_s))
		{
			our_pos += camera_orientation.rotate_vector(Vec3f(0.0f, 0.0f, -100.0f)) * game_time.get_time_elapsed();
		}

		// Check for collision between camera and scene:

		float zoom_out = 250.0f;
		Vec3f camera_look_pos = our_pos;
		Vec3f camera_pos = camera_look_pos + camera_orientation.rotate_vector(Vec3f(0.0f, 0.0f, -zoom_out));

		SphereShape sphere(camera_look_pos, 5.0f);
		float t = TriangleMeshShape::sweep(&shape, &sphere, camera_pos);
		camera_pos = mix(camera_look_pos, camera_pos, t);

//		for (int i = 0; i <= 1000; i++)
//		{
//			SphereShape sphere(mix(camera_look_pos, camera_pos, i / 1000.0f), 5.0f);
//			if (TriangleMeshShape::find_any_hit(&shape, &sphere))
//			{
//				camera_pos = mix(camera_look_pos, camera_pos, std::max((i-1) / 1000.0f, 0.0f));
//				break;
//			}
//		}

		// Update scene camera:

		camera.set_orientation(camera_orientation);
		camera.set_position(camera_pos);

		model_box.set_position(our_pos);

		scene.update(gc, game_time.get_time_elapsed());
		scene.set_viewport(gc.get_size());
		scene.render(gc);

		window.flip(1);
	}

	return 0;
}
*/
