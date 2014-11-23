
#pragma once

#include "undo_system.h"
#include "Commands/update_animation_command.h"

class AppModel
{
public:
	AppModel();
	~AppModel();

	static AppModel *instance();

	const std::shared_ptr<clan::FBXModel> &fbx() const { return _fbx; }
	const std::shared_ptr<clan::ModelData> &model_data() const { return _model_data; }

	UndoSystem undo_system;

	std::vector<clan::FBXAttachmentPoint> attachment_points;
	std::vector<clan::FBXParticleEmitter> particle_emitters;
	std::vector<clan::FBXAnimation> animations;

	void open(const std::string &filename);
	void update_scene_model();

	clan::Signal<void()> &sig_model_data_updated() { return _sig_model_data_updated; }

private:
	std::shared_ptr<clan::FBXModel> _fbx;
	std::shared_ptr<clan::ModelData> _model_data;
	clan::Signal<void()> _sig_model_data_updated;

	static AppModel *instance_ptr;
};
