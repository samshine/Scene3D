
#include "precomp.h"
#include "asset_folder_model.h"

using namespace uicore;

AssetFolderModel *AssetFolderModel::instance()
{
	static AssetFolderModel model;
	return &model;
}

void AssetFolderModel::open(const std::string &new_folder_path)
{
	project_folder = new_folder_path;
	assets_folder = FilePath::combine(project_folder, "Assets");
	output_folder = FilePath::combine(project_folder, "Resources/Assets");
	sig_open_finished();
}
