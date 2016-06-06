
#include "precomp.h"
#include "demo_screen_controller.h"

using namespace uicore;

DemoScreenController::DemoScreenController()
{
	scene = Scene::create(scene_engine());
	scene->show_skybox_stars(false);
//	scene->set_skybox_gradient(std::vector<Colorf> { StandardColorf::darkslategray(), StandardColorf::darkslateblue() });
	scene->set_skybox_gradient(std::vector<Colorf> { StandardColorf::black(), StandardColorf::black() });
	camera = SceneCamera::create(scene);

//	box = SceneObject::create(scene, SceneModel::create(scene, ModelData::create_box(Vec3f{ 1.0f, 1.0f, 1.0f })));
//	box->set_position(Vec3f{ 0.0f, 0.0f, 10.0f });

	light1 = SceneLight::create(scene);
	light1->set_position(Vec3f{ 10.0f, 10.0f, 10.0f });

	light2 = SceneLight::create(scene);
	light2->set_position(Vec3f{ -10.0f, -10.0f, -10.0f });

	font = Font::create("Segoe UI", 13.0f);

	particle_texture = Texture2D::create(gc(), "Resources/particle2.png");

	compile_emitter_program();
	compile_particle_update_program();
	compile_particle_render_program();

	std::vector<Particle> emitters(emitter_count);
	for (int i = 0; i < emitter_count; i++)
	{
		auto &emitter = emitters[i];
		emitter.pos = Vec3f(0, 0, 70) + Vec3f(-std::sin(radians((float)i)), -std::cos(radians((float)i)), -std::sin(radians((float)i))) * 4.0f;
		emitter.life = 1;
		emitter.particle_subarray_start = particle_count * i / emitter_count;
		emitter.particle_subarray_size = particle_count / emitter_count;
		emitter.emit_position = 0;
		emitter.emit_cooldown = 0;
	}

	emitters_storage = StorageBuffer::create(gc(), emitters.data(), sizeof(Particle) * emitter_count, sizeof(Particle));
	particles_storage = StorageBuffer::create(gc(), sizeof(Particle) * particle_count, sizeof(Particle));
}

void DemoScreenController::compile_emitter_program()
{
	auto shader = ShaderObject::create(gc(), ShaderType::compute, File::read_all_text("emitter.hlsl"));
	shader->compile();

	emitter_program = ProgramObject::create(gc());
	emitter_program->attach(shader);
	emitter_program->link();
	emitter_program->set_storage_buffer_index("emitters", 0);
	emitter_program->set_storage_buffer_index("particles", 1);
}

void DemoScreenController::compile_particle_update_program()
{
	auto shader = ShaderObject::create(gc(), ShaderType::compute, File::read_all_text("particle.hlsl"));
	shader->compile();

	particle_update_program = ProgramObject::create(gc());
	particle_update_program->attach(shader);
	particle_update_program->link();
	particle_update_program->set_storage_buffer_index("particles", 0);
}

void DemoScreenController::compile_particle_render_program()
{
	auto vertex_shader = ShaderObject::create(gc(), ShaderType::vertex, File::read_all_text("particle_vertex.hlsl"));
	vertex_shader->compile();

	auto fragment_shader = ShaderObject::create(gc(), ShaderType::fragment, File::read_all_text("particle_fragment.hlsl"));
	fragment_shader->compile();

	particle_render_program = ProgramObject::create(gc());
	particle_render_program->attach(vertex_shader);
	particle_render_program->attach(fragment_shader);
	particle_render_program->bind_attribute_location(0, "AttrPosition");
	particle_render_program->bind_frag_data_location(0, "FragColor");
	particle_render_program->link();
	particle_render_program->set_uniform_buffer_index("Uniforms", 0);
	particle_render_program->set_uniform1i("ParticleTexture", 0);
	particle_render_program->set_uniform1i("ParticleSampler", 0);
	particle_render_program->set_storage_buffer_index("particles", 0);

	Vec3f cpu_billboard_positions[6] =
	{
		Vec3f(-1.0f, -1.0f, 0.0f),
		Vec3f(1.0f, -1.0f, 0.0f),
		Vec3f(-1.0f,  1.0f, 0.0f),
		Vec3f(-1.0f,  1.0f, 0.0f),
		Vec3f(1.0f, -1.0f, 0.0f),
		Vec3f(1.0f,  1.0f, 0.0f)
	};
	billboard_positions = VertexArrayVector<Vec3f>(gc(), cpu_billboard_positions, 6);

	BlendStateDescription blend_desc;
	blend_desc.enable_blending(true);
	blend_desc.set_blend_function(blend_src_alpha, blend_one_minus_src_alpha, blend_zero, blend_zero);
	//blend_desc.set_blend_function(blend_one, blend_one, blend_zero, blend_one);
	blend_state = gc()->create_blend_state(blend_desc);

	DepthStencilStateDescription depth_stencil_desc;
	depth_stencil_desc.enable_depth_write(false);
	depth_stencil_desc.enable_depth_test(true);
	depth_stencil_desc.set_depth_compare_function(compare_lequal);
	depth_stencil_state = gc()->create_depth_stencil_state(depth_stencil_desc);

	RasterizerStateDescription rasterizer_desc;
	rasterizer_desc.set_culled(false);
	rasterizer_state = gc()->create_rasterizer_state(rasterizer_desc);

	prim_array = PrimitivesArray::create(gc());
	prim_array->set_attributes(0, billboard_positions);

	gpu_uniforms = UniformVector<Uniforms>(gc(), 1);
}

void DemoScreenController::update()
{
	float rotate = game_time().time_elapsed() * 100.0f;
//	box->set_orientation(box->orientation() * Quaternionf::euler(Vec3f(radians(rotate)), EulerOrder::xyz));

	gc()->set_program_object(emitter_program);
	gc()->set_storage_buffer(0, emitters_storage);
	gc()->set_storage_buffer(1, particles_storage);
	gc()->dispatch(emitter_count);
	gc()->set_storage_buffer(0, nullptr);
	gc()->set_storage_buffer(1, nullptr);
	gc()->set_program_object(nullptr);

	gc()->set_program_object(particle_update_program);
	gc()->set_storage_buffer(0, particles_storage);
	gc()->dispatch(particle_count);
	gc()->set_storage_buffer(0, nullptr);
	gc()->set_program_object(nullptr);

	scene_engine()->set_custom_pass([&](Mat4f eye_to_projection, Mat4f world_to_eye, FrameBufferPtr fb_final_color, Sizef viewport_size)
	{
		Uniforms uniforms;
		uniforms.eye_to_projection = eye_to_projection;
		uniforms.object_to_eye = world_to_eye;
		gpu_uniforms.upload_data(gc(), &uniforms, 1);

		gc()->set_depth_range(0.0f, 0.9f);

		gc()->set_frame_buffer(fb_final_color);
		gc()->set_viewport(viewport_size, gc()->texture_image_y_axis());
		gc()->set_depth_stencil_state(depth_stencil_state);
		gc()->set_blend_state(blend_state);
		gc()->set_rasterizer_state(rasterizer_state);
		gc()->set_primitives_array(prim_array);
		gc()->set_storage_buffer(0, particles_storage);
		gc()->set_program_object(particle_render_program);

		gc()->set_uniform_buffer(0, gpu_uniforms);
		gc()->set_texture(0, particle_texture);

		gc()->draw_primitives_array_instanced(type_triangles, 0, 6, particle_count);

		gc()->reset_primitives_array();
		gc()->reset_rasterizer_state();
		gc()->reset_depth_stencil_state();
		gc()->reset_program_object();
		gc()->reset_primitives_elements();
		gc()->reset_texture(0);
		gc()->reset_storage_buffer(0);
		gc()->reset_uniform_buffer(0);
		gc()->reset_frame_buffer();

		gc()->set_depth_range(0.0f, 1.0f);
	});

	scene_viewport()->set_camera(camera);
	scene_viewport()->render(gc());

	canvas()->begin();

	//font->draw_text(canvas(), 100.0f, 100.0f, string_format("Particle pos: %1, %2, %3", pos.x, pos.y, pos.z));

	//auto img = Image::create(collision->gpu_output_image, collision->gpu_output_image->size());
	//img->draw(canvas(), 100.0f, 150.0f);

	canvas()->end();
}
