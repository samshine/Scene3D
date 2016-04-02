
#pragma once

#include "Model/UndoSystem/undo_system.h"
#include "Commands/add_animation_command.h"
#include "Commands/add_attachment_command.h"
#include "Commands/add_material_command.h"
#include "Commands/add_bone_command.h"
#include "Commands/update_animation_command.h"
#include "Commands/update_attachment_command.h"
#include "Commands/update_material_command.h"
#include "Commands/update_bone_command.h"

class ModelAppModel
{
public:
	ModelAppModel();
	~ModelAppModel();

	static ModelAppModel *instance();

	std::string open_filename;
	ModelDesc desc;
	std::string map_model;

	FBXModelPtr fbx;
	std::shared_ptr<ModelData> model_data;

	UndoSystem undo_system;

	uicore::Signal<void()> sig_load_finished;
	uicore::Signal<void()> sig_model_data_updated;
	uicore::Signal<void()> sig_map_model_updated;

	void open(const std::string &filename);
	void save(const std::string &filename);
	void set_fbx_model(const std::string &filename);
	void update_scene_model();

private:
	static ModelAppModel *instance_ptr;
};
