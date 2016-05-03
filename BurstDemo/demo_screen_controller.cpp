
#include "precomp.h"
#include "demo_screen_controller.h"

using namespace uicore;

DemoScreenController::DemoScreenController()
{
	scene = Scene::create(scene_engine());
	scene->show_skybox_stars(false);
	scene->set_skybox_gradient(std::vector<Colorf> { Colorf::darkslategray, Colorf::darkslateblue });

	camera = SceneCamera::create(scene);

	box = SceneObject::create(scene, SceneModel::create(scene, ModelData::create_box(Vec3f{ 1.0f, 1.0f, 1.0f })));
	box->set_position(Vec3f{ 0.0f, 0.0f, 10.0f });

	light1 = SceneLight::create(scene);
	light1->set_position(Vec3f{ 10.0f, 10.0f, 10.0f });

	light2 = SceneLight::create(scene);
	light2->set_position(Vec3f{ -10.0f, -10.0f, -10.0f });

	font = Font::create("Segoe UI", 13.0f);

	auto shader = ShaderObject::create(gc(), ShaderType::compute, File::read_all_text("particle.hlsl"));
	shader->compile();

	program = ProgramObject::create(gc());
	program->attach(shader);
	program->link();
	program->set_storage_buffer_index("particles", 0);

	storage = StorageBuffer::create(gc(), sizeof(Particle) * particle_count, sizeof(Particle));
	staging = StagingBuffer::create(gc(), sizeof(Particle) * particle_count);
}

void DemoScreenController::update()
{
	float rotate = game_time().time_elapsed() * 100.0f;
	box->set_orientation(box->orientation() * Quaternionf(rotate, rotate, rotate, angle_degrees, order_XYZ));

	scene_viewport()->set_camera(camera);
	scene_viewport()->render(gc());

	gc()->set_program_object(program);
	gc()->set_storage_buffer(0, storage);
	gc()->dispatch(particle_count);
	gc()->set_storage_buffer(0, nullptr);
	gc()->set_program_object(nullptr);

	storage->copy_to(gc(), staging);

	staging->lock(gc(), access_read_write);

	Particle *particles = (Particle*)staging->data();
	auto pos = particles[0].pos;
	particles[0].velocity = Vec3f(1.0f, 0.0f, 0.0f);

	staging->unlock();

	storage->copy_from(gc(), staging);

	canvas()->begin();
	font->draw_text(canvas(), 100.0f, 100.0f, string_format("Particle pos: %1, %2, %3", pos.x, pos.y, pos.z));
	canvas()->end();
}
