
#pragma once

#include "Model/UndoSystem/undo_system.h"
#include "Commands/add_map_object_command.h"
#include "Commands/update_map_object_command.h"
#include "Commands/remove_map_object_command.h"
#include "Commands/set_map_object_mesh_command.h"
#include "Commands/add_map_material_command.h"
#include "Commands/update_map_material_command.h"

class EditorScene;

enum class MapToolType
{
	select,
	select_move,
	select_rotate,
	select_scale,
	create_object,
	create_trigger,
	create_path_point,
	create_light_probe
};

class MapAppModel
{
public:
	MapAppModel();
	~MapAppModel();

	static MapAppModel *instance();

	std::string open_filename;
	MapDesc desc;

	MapDescObject create_object_template;

	UndoSystem undo_system;

	MapToolType tool = MapToolType::select_move;
	std::vector<size_t> selected_objects;

	uicore::Signal<void()> sig_load_finished;
	uicore::Signal<void()> sig_map_updated;
	uicore::Signal<void()> sig_map_tool_changed;
	uicore::Signal<void(size_t index)> sig_object_added;
	uicore::Signal<void(size_t index)> sig_object_removed;
	uicore::Signal<void(size_t index)> sig_object_updated;
	uicore::Signal<void(size_t index)> sig_object_selected;
	uicore::Signal<void(size_t index)> sig_object_unselected;

	void open(const std::string &filename);
	void save(const std::string &filename);

private:
	static MapAppModel *instance_ptr;
};
