
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
	desc = MapDesc::load(filename);
	open_filename = filename;
	map_fbx = std::make_shared<FBXModel>(desc.fbx_filename);
	update_map_model_data();
	sig_load_finished();
}

void MapAppModel::save(const std::string &filename)
{
	desc.save(filename);
	open_filename = filename;
}

void MapAppModel::update_map_model_data()
{
	if (map_fbx)
		map_model_data = map_fbx->convert(desc);
	else
		map_model_data.reset();

	sig_map_model_data_updated();
}

MapAppModel *MapAppModel::instance()
{
	return instance_ptr;
}

MapAppModel *MapAppModel::instance_ptr = 0;
