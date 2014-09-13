
#include "app_model.h"
#include "FBXModel/fbx_model.h"
#include "FBXModel/fbx_model_loader.h"
#include <memory>

AppModel::AppModel()
{
	if (instance_ptr) throw std::exception();
	instance_ptr = this;
}

AppModel::~AppModel()
{
	instance_ptr = 0;
}

void AppModel::open(const std::string &filename)
{
	_fbx = std::make_shared<FBXModel>(filename);
	animations.clear();
	animations.resize(1);
	animations.at(0).name = "default";
	update_scene_model();
}

void AppModel::update_scene_model()
{
	_model_data = _fbx->convert(animations, attachment_points, particle_emitters);
	_sig_model_data_updated();
}

AppModel *AppModel::instance()
{
	return instance_ptr;
}

AppModel *AppModel::instance_ptr = 0;
