
#pragma once

#include "Views/AssetCompilerWindow/asset_explorer_view.h"

class AssetExplorerController
{
public:
	AssetExplorerController();

	std::shared_ptr<AssetExplorerView> view = std::make_shared<AssetExplorerView>();

private:
	void populate_list_view();
	void add_items(TreeItemPtr parent, const std::string &path);

	uicore::SlotContainer slots;
};
