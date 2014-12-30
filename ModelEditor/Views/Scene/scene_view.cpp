
#include "precomp.h"
#include "scene_view.h"
#include "viewer_scene_cache.h"

using namespace clan;

SceneView::SceneView()
{
	box_style.set_layout_hbox();
	box_style.set_flex(1.0f, 1.0f);

	set_focus_policy(FocusPolicy::accept);

	slots.connect(sig_pointer_press(), [this](PointerEvent &e)
	{
		set_focus();
	});

	timer = Timer();
	timer.func_expired() = [this]()
	{
		set_needs_render();
	};
}

std::string SceneView::get_animation() const
{
	if (!object1.is_null())
		return object1.get_animation();
	else
		return current_animation;
}

void SceneView::play_animation(const std::string &name, bool instant)
{
	current_animation = name;
	if (!object1.is_null())
		object1.play_animation(current_animation, instant);
}

void SceneView::play_transition(const std::string &anim1, const std::string &anim2, bool instant)
{
	current_animation = anim2;
	if (!object1.is_null())
		object1.play_transition(anim1, anim2, instant);
}

void SceneView::set_map_model(const std::string &new_map_model)
{
	map_model_filename = new_map_model;
	map_model_updated = true;
}

void SceneView::set_model_data(std::shared_ptr<ModelData> new_model_data)
{
	model_data = new_model_data;
	model_data_updated = true;
	set_needs_render();
}

void SceneView::set_attachments(std::vector<SceneViewAttachment> new_attachments)
{
	attachments = new_attachments;
	model_data_updated = true;
	set_needs_render();
}

void SceneView::render_content(Canvas &canvas)
{
	Pointf viewport_pos = Vec2f(canvas.get_transform() * Vec4f(0.0f, 0.0f, 0.0f, 1.0f));
	Sizef viewport_size = geometry().content.get_size();
	//Size viewport_size_i = Size(2400*2 + rand() % 160 * 2, 1300*2 + rand() % 160 * 2);
	Size viewport_size_i = Size(viewport_size) * 2;

	canvas.flush();
	GraphicContext gc = canvas.get_gc();
	InputContext ic;
	if (dynamic_cast<WindowView*>(root_view())) ic = static_cast<WindowView*>(root_view())->get_display_window().get_ic();

	setup_scene(gc);
	sig_update_scene(scene, gc, ic);
	update_map(gc);
	update_model(gc);

	camera.set_orientation(Quaternionf(up, dir, 0.0f, angle_degrees, order_YXZ));
	camera.set_position(camera.get_orientation().rotate_vector(Vec3f(0.0f, 10.0f, -25.0f)));

	gametime.update();

	if (!object1.is_null())
	{
		object1.update(gametime.get_time_elapsed());
		for (auto &attachment : attachments)
		{
			if (attachment.object.is_null())
				continue;

			Vec3f attach_pos, attach_scale;
			Quaternionf attach_orientation;
			object1.get_attachment_location(attachment.attachment_name, attach_pos, attach_orientation, attach_scale);
			attachment.object.set_position(attach_pos);
			attachment.object.set_orientation(attach_orientation);
			attachment.object.set_scale(attach_scale * attachment.model_scale);
			attachment.object.update(gametime.get_time_elapsed());
		}
	}
	scene.update(gc, gametime.get_time_elapsed());

	if (scene_frame_buffer.is_null() || scene_texture.is_null() || scene_texture.get_size() != viewport_size_i)
	{
		scene_frame_buffer = FrameBuffer();
		scene_texture = Texture2D();
		gc.flush();

		scene_texture = Texture2D(gc, viewport_size_i);
		scene_frame_buffer = FrameBuffer(gc);
		scene_frame_buffer.attach_color(0, scene_texture);
	}

	//scene.set_viewport(RectfPS(viewport_pos.x, viewport_pos.y, viewport_size.width, viewport_size.height));
	scene.set_viewport(viewport_size_i, scene_frame_buffer);
	scene.render(gc);

	gc.set_viewport(gc.get_size());

	canvas.fill_triangles(
		{
			Vec2f(0.0f, 0.0f), Vec2f(viewport_size_i.width / 2.0f, 0.0f), Vec2f(0.0f, viewport_size_i.height / 2.0f),
			Vec2f(viewport_size_i.width / 2.0f, 0.0f), Vec2f(0.0f, viewport_size_i.height / 2.0f), Vec2f(viewport_size_i.width / 2.0f, viewport_size_i.height / 2.0f)
		}, scene_texture);

	timer.start(10, false);
}

void SceneView::update_map(GraphicContext &gc)
{
	if (!map_model_updated) return;
	map_model_updated = false;

	map_model = SceneModel();
	map_object = SceneObject();

	try
	{
		FBXModelDesc model_desc = FBXModelDesc::load(map_model_filename);

		FBXModel fbx_model(model_desc.fbx_filename);
		auto attachment_model_data = fbx_model.convert(model_desc);

		map_model = SceneModel(gc, scene, attachment_model_data);
		map_object = SceneObject(scene, map_model);
	}
	catch (Exception &)
	{
	}
}

void SceneView::update_model(GraphicContext &gc)
{
	if (!model_data_updated) return;
	model_data_updated = false;

	model1 = SceneModel();
	object1 = SceneObject();

	if (model_data)
	{
		model1 = SceneModel(gc, scene, model_data);
		object1 = SceneObject(scene, model1, Vec3f(), Quaternionf(), Vec3f(1.0f));
		object1.play_animation(current_animation, true);

		for (auto &attachment : attachments)
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
	else
	{
		model1 = SceneModel();
		object1 = SceneObject();
	}
}

void SceneView::setup_scene(GraphicContext &gc)
{
	if (!scene.is_null()) return;

	resources = ResourceManager();
	SceneCache::set(resources, std::shared_ptr<SceneCache>(new ViewerSceneCache()));

	scene = Scene(gc, resources, "Resources/Scene3D");

	scene.show_skybox_stars(false);
	std::vector<Colorf> gradient;
	gradient.push_back(Colorf(236 * 5 / 10, 240 * 5 / 10, 243 * 5 / 10));
	gradient.push_back(Colorf(236 * 5 / 10, 240 * 5 / 10, 243 * 5 / 10));
	gradient.push_back(Colorf(236 * 5 / 10, 240 * 5 / 10, 243 * 5 / 10));
	gradient.push_back(Colorf(236 * 5 / 10, 240 * 5 / 10, 243 * 5 / 10));
	gradient.push_back(Colorf(236 * 5 / 10, 240 * 5 / 10, 243 * 5 / 10));
	gradient.push_back(Colorf(236 * 5 / 10, 240 * 5 / 10, 243 * 5 / 10));
	gradient.push_back(Colorf(236 * 6 / 10, 240 * 6 / 10, 243 * 6 / 10));
	gradient.push_back(Colorf(236 * 7 / 10, 240 * 7 / 10, 243 * 7 / 10));
	gradient.push_back(Colorf(236 * 8 / 10, 240 * 8 / 10, 243 * 8 / 10));
	for (auto &g : gradient)
	{
		g.r = std::pow(g.r, 2.2f);
		g.g = std::pow(g.g, 2.2f);
		g.b = std::pow(g.b, 2.2f);
	}
	scene.set_skybox_gradient(gc, gradient);

	light1 = SceneLight(scene);
	light1.set_position(Vec3f(30.0f, 42.0f, -30.0f));
	light1.set_orientation(Quaternionf(45.0f, 315.0f, 0.0f, angle_degrees, order_YXZ));
	light1.set_type(SceneLight::type_spot);
	light1.set_attenuation_start(900.0f);
	light1.set_attenuation_end(1000.0f);
	light1.set_color(Vec3f(1.0f, 0.95f, 0.90f));
	light1.set_shadow_caster(true);
	light1.set_hotspot(10.0f);
	light1.set_falloff(45.0f);
	
	light2 = SceneLight(scene);
	light2.set_position(Vec3f(-100.0f, -100.0f, -100.0f));
	light2.set_type(SceneLight::type_omni);
	light2.set_attenuation_start(900.0f);
	light2.set_attenuation_end(1000.0f);
	light2.set_color(Vec3f(0.1f, 0.1f, 0.12f));

	camera = SceneCamera(scene);
	scene.set_camera(camera);

	gametime.reset();
}
