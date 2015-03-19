
#include "precomp.h"
#include "map_app_model.h"
#include <memory>

using namespace clan;

MapAppModel::MapAppModel()
{
	if (instance_ptr) throw std::exception();
	instance_ptr = this;
}

MapAppModel::~MapAppModel()
{
	instance_ptr = 0;
}

void MapAppModel::open(const std::string &filename)
{
	desc = FBXModelDesc::load(filename);
	open_filename = filename;
	set_fbx_model(desc.fbx_filename);
	sig_load_finished();
}

void MapAppModel::save(const std::string &filename)
{
	desc.save(filename);
	open_filename = filename;
}

void MapAppModel::set_fbx_model(const std::string &filename)
{
	desc.fbx_filename = filename;
	fbx = std::make_shared<FBXModel>(desc.fbx_filename);
	update_scene_model();
}

void MapAppModel::update_scene_model()
{
	if (fbx)
		model_data = fbx->convert(desc);
	else
		model_data.reset();

	sig_model_data_updated();
}

MapAppModel *MapAppModel::instance()
{
	return instance_ptr;
}

MapAppModel *MapAppModel::instance_ptr = 0;
