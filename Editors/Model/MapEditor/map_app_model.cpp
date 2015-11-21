
#include "precomp.h"
#include "map_app_model.h"
#include <memory>

using namespace uicore;

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
	sig_map_updated();
	sig_load_finished();
}

void MapAppModel::save(const std::string &filename)
{
	desc.save(filename);
	open_filename = filename;
}

MapAppModel *MapAppModel::instance()
{
	return instance_ptr;
}

MapAppModel *MapAppModel::instance_ptr = 0;
