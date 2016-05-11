
#include "precomp.h"
#include "demo_screen_controller.h"
#include "gpu_collision.h"

using namespace uicore;

DemoScreenController::DemoScreenController()
{
	scene = Scene::create(scene_engine());
	scene->show_skybox_stars(false);
//	scene->set_skybox_gradient(std::vector<Colorf> { Colorf::darkslategray, Colorf::darkslateblue });
	scene->set_skybox_gradient(std::vector<Colorf> { Colorf::black, Colorf::black });
	camera = SceneCamera::create(scene);

//	box = SceneObject::create(scene, SceneModel::create(scene, ModelData::create_box(Vec3f{ 1.0f, 1.0f, 1.0f })));
//	box->set_position(Vec3f{ 0.0f, 0.0f, 10.0f });

	light1 = SceneLight::create(scene);
	light1->set_position(Vec3f{ 10.0f, 10.0f, 10.0f });

	light2 = SceneLight::create(scene);
	light2->set_position(Vec3f{ -10.0f, -10.0f, -10.0f });

	font = Font::create("Segoe UI", 13.0f);

	particle_texture = Texture2D::create(gc(), "Resources/particle2.png");

	auto shader = ShaderObject::create(gc(), ShaderType::compute, File::read_all_text("particle.hlsl"));
	shader->compile();

	program = ProgramObject::create(gc());
	program->attach(shader);
	program->link();
	program->set_storage_buffer_index("particles", 0);

	storage = StorageBuffer::create(gc(), sizeof(Particle) * particle_count, sizeof(Particle));
//	staging = StagingBuffer::create(gc(), sizeof(Particle) * particle_count);

	collision = std::make_shared<GPUCollision>(gc());
	collision->set_shape(ModelData::create_box(Vec3f{ 20.0f, 20.0f, 5.0f }));

	auto vertex_shader = ShaderObject::create(gc(), ShaderType::vertex, File::read_all_text("particle_vertex.hlsl"));
	vertex_shader->compile();

	auto fragment_shader = ShaderObject::create(gc(), ShaderType::fragment, File::read_all_text("particle_fragment.hlsl"));
	fragment_shader->compile();

	program2 = ProgramObject::create(gc());
	program2->attach(vertex_shader);
	program2->attach(fragment_shader);
	program2->bind_attribute_location(0, "AttrPosition");
	program2->bind_frag_data_location(0, "FragColor");
	program2->link();
	program2->set_uniform_buffer_index("Uniforms", 0);
	program2->set_uniform1i("ParticleTexture", 0);
	program2->set_uniform1i("ParticleSampler", 0);
	program2->set_storage_buffer_index("particles", 0);

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

	gc()->set_program_object(program);
	gc()->set_storage_buffer(0, storage);
	gc()->dispatch(particle_count);
	gc()->set_storage_buffer(0, nullptr);
	gc()->set_program_object(nullptr);

	/*
	storage->copy_to(gc(), staging);

	staging->lock(gc(), access_read_write);

	Particle *particles = (Particle*)staging->data();
	auto pos = particles[0].pos;
	particles[0].size = 1.0f;
	particles[0].velocity = Vec3f(0.01f, 0.0f, 0.1f);

	staging->unlock();

	storage->copy_from(gc(), staging);
	*/

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
		gc()->set_storage_buffer(0, storage);
		gc()->set_program_object(program2);

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

	collision->update();

	canvas()->begin();
	//font->draw_text(canvas(), 100.0f, 100.0f, string_format("Particle pos: %1, %2, %3", pos.x, pos.y, pos.z));

//	auto img = Image::create(collision->gpu_output_image, collision->gpu_output_image->size());
//	img->draw(canvas(), 100.0f, 150.0f);

	canvas()->end();
}
