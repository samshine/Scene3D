
#pragma once

#include "Controllers/SidePanel/side_panel_controller.h"

class MapSelectRolloutView;
class MapCreateRolloutView;
class MapObjectRolloutView;
class MapObjectListRolloutView;

class MapEditController : public SidePanelController
{
public:
	MapEditController();

private:
	void map_tool_changed();

	void select_clicked();
	void select_move_clicked();
	void select_rotate_clicked();
	void select_scale_clicked();
	void create_object_clicked();
	void create_trigger_clicked();
	void create_path_point_clicked();
	void create_light_probe_clicked();

	void create_object_id_value_changed();
	void create_object_type_value_changed();
	void create_object_dir_value_changed();
	void create_object_up_value_changed();
	void create_object_tilt_value_changed();
	void create_object_scale_value_changed();
	void create_object_mesh_browse();
	void create_object_field_value_changed();

	void object_updated(size_t index);
	void update_objects();
	void objects_list_selection_changed();

	void update_object_fields();
	void edit_object_id_value_changed();
	void edit_object_type_value_changed();
	void edit_object_position_value_changed();
	void edit_object_dir_value_changed();
	void edit_object_up_value_changed();
	void edit_object_tilt_value_changed();
	void edit_object_scale_value_changed();
	void edit_object_mesh_browse();
	void edit_object_field_value_changed();

	uicore::SlotContainer slots;

	std::shared_ptr<MapSelectRolloutView> select;
	std::shared_ptr<MapCreateRolloutView> create;
	std::shared_ptr<MapObjectRolloutView> create_object;
	std::shared_ptr<MapObjectListRolloutView> objects;
	std::shared_ptr<MapObjectRolloutView> edit_object;
};
