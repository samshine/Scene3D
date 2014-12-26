
#include "precomp.h"
#include "app_model.h"
#include <memory>

using namespace clan;

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
	desc = FBXModelDesc::load(filename);
	open_filename = filename;
	set_fbx_model(desc.fbx_filename);
	sig_load_finished();
}

void AppModel::save(const std::string &filename)
{
	desc.save(filename);
	open_filename = filename;
}

void AppModel::set_fbx_model(const std::string &filename)
{
	desc.fbx_filename = filename;
	fbx = std::make_shared<FBXModel>(desc.fbx_filename);
	update_scene_model();
}

void AppModel::update_scene_model()
{
	if (fbx)
		model_data = fbx->convert(desc);
	else
		model_data.reset();

	sig_model_data_updated();
}

AppModel *AppModel::instance()
{
	return instance_ptr;
}

AppModel *AppModel::instance_ptr = 0;
