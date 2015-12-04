
#include "precomp.h"
#include "map_scene_create_object_action.h"
#include "map_scene_controller.h"
#include "Views/Scene/scene_view.h"
#include "Model/MapEditor/map_app_model.h"

using namespace uicore;

MapSceneCreateObjectAction::MapSceneCreateObjectAction(MapSceneController *controller, int view_index) : controller(controller), view_index(view_index)
{
}

void MapSceneCreateObjectAction::pointer_press(PointerEvent &e)
{
	auto app_model = MapAppModel::instance();
	if (app_model->tool == MapToolType::create_object && e.button() == PointerButton::left)
	{
		begin_action();

		auto scene_view = controller->scene_views[view_index];

		Vec3f ray_start, ray_direction;
		scene_view->unproject(e.pos(scene_view), ray_start, ray_direction);

		auto hit = controller->collision->ray_test_nearest(ray_start, ray_start + ray_direction * 100.0f);
		if (hit)
		{
			MapDescObject obj = app_model->create_object_template;
			obj.position = hit.position;
			app_model->undo_system.execute<AddMapObjectCommand>(obj);
		}
	}
}

void MapSceneCreateObjectAction::pointer_release(PointerEvent &e)
{
	if (action_active() && e.button() == PointerButton::left)
		end_action();
}

void MapSceneCreateObjectAction::pointer_move(PointerEvent &e)
{
}
