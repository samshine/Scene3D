
#pragma once

#include <ClanLib/core.h>
#include <ClanLib/display.h>
#include <ClanLib/scene3d.h>
#include <UICore/Signals/signal.h>
#include "attachment_point.h"
#include "particle_emitter.h"
#include "animation.h"
#include "undo_system.h"
#include "Commands/update_animation_command.h"

class FBXModel;

class AppModel
{
public:
	AppModel();
	~AppModel();

	static AppModel *instance();

	const std::shared_ptr<FBXModel> &fbx() const { return _fbx; }
	const std::shared_ptr<clan::ModelData> &model_data() const { return _model_data; }

	UndoSystem undo_system;

	std::vector<AttachmentPoint> attachment_points;
	std::vector<ParticleEmitter> particle_emitters;
	std::vector<Animation> animations;

	void open(const std::string &filename);
	void update_scene_model();

	uicore::Signal<void()> &sig_model_data_updated() { return _sig_model_data_updated; }

private:
	std::shared_ptr<FBXModel> _fbx;
	std::shared_ptr<clan::ModelData> _model_data;
	uicore::Signal<void()> _sig_model_data_updated;

	static AppModel *instance_ptr;
};
