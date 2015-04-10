
#pragma once

#include "Model/UndoSystem/undo_system.h"
#include "Commands/add_map_material_command.h"
#include "Commands/update_map_material_command.h"

class EditorScene;

class MapAppModel
{
public:
	MapAppModel();
	~MapAppModel();

	static MapAppModel *instance();

	std::string open_filename;
	clan::MapDesc desc;

	std::shared_ptr<clan::FBXModel> fbx;
	std::shared_ptr<clan::ModelData> model_data;

	UndoSystem undo_system;

	clan::Signal<void()> sig_load_finished;
	clan::Signal<void()> sig_model_data_updated;
	clan::Signal<void()> sig_map_model_updated;

	void open(const std::string &filename);
	void save(const std::string &filename);
	void set_fbx_model(const std::string &filename);
	void update_scene_model();

private:
	static MapAppModel *instance_ptr;
};
