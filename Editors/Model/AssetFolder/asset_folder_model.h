
#pragma once

class AssetFolderModel
{
public:
	static AssetFolderModel *instance();

	std::string project_folder;
	std::string assets_folder;
	std::string output_folder;

	uicore::Signal<void()> sig_open_finished;

	void open(const std::string &folder_path);
};
