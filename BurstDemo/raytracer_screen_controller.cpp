
#include "precomp.h"
#include "raytracer_screen_controller.h"
#include "gpu_collision.h"

using namespace uicore;

RaytracerScreenController::RaytracerScreenController()
{
	font = Font::create("Segoe UI", 13.0f);

	collision = std::make_shared<GPUCollision>(gc());
	//collision->set_shape(ModelData::create_box(Vec3f{ 20.0f, 20.0f, 5.0f } *0.01f));
	collision->set_shape(ModelData::load("C:\\Development\\Workspaces\\Scene3D\\ShooterGame\\Resources\\Assets\\Models\\XBot\\XBot.cmodel"));
}

void RaytracerScreenController::update()
{
	collision->update();

	canvas()->begin();
	canvas()->clear();

	font->draw_text(canvas(), 100.0f, 100.0f, string_format("Frame duration: %1 ms", Screen::instance()->last_frame_stats().frame_ms));

	auto img = Image::create(collision->gpu_output_image, collision->gpu_output_image->size());
	img->draw(canvas(), (canvas()->width() - img->width()) * 0.5f, (canvas()->height() - img->height()) * 0.5f);

	canvas()->end();
}
