
#pragma once

#include "undo_system.h"
#include "Commands/update_animation_command.h"
#include "Commands/update_attachment_command.h"

class AppModel
{
public:
	AppModel();
	~AppModel();

	static AppModel *instance();

	std::string open_filename;
	clan::FBXModelDesc desc;
	std::shared_ptr<clan::FBXModel> fbx;
	std::shared_ptr<clan::ModelData> model_data;

	UndoSystem undo_system;

	clan::Signal<void()> sig_load_finished;
	clan::Signal<void()> sig_model_data_updated;

	void open(const std::string &filename);
	void save(const std::string &filename);
	void set_fbx_model(const std::string &filename);
	void update_scene_model();

private:
	static AppModel *instance_ptr;
};
