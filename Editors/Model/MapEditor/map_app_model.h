
#pragma once

#include "Model/UndoSystem/undo_system.h"
#include "Commands/set_map_fbx_filename_command.h"
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

	std::shared_ptr<FBXModel> map_fbx;
	std::shared_ptr<ModelData> map_model_data;

	UndoSystem undo_system;

	MapToolType tool = MapToolType::select_move;

	uicore::Signal<void()> sig_load_finished;
	uicore::Signal<void()> sig_map_model_data_updated;
	uicore::Signal<void()> sig_map_tool_changed;

	void open(const std::string &filename);
	void save(const std::string &filename);

	void update_map_model_data();

private:
	static MapAppModel *instance_ptr;
};
