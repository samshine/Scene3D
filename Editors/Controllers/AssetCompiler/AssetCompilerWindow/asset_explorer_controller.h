
#pragma once

#include "Views/AssetCompilerWindow/asset_explorer_view.h"

class AssetExplorerController
{
public:
	AssetExplorerController();

	std::shared_ptr<AssetExplorerView> view = std::make_shared<AssetExplorerView>();

private:
	uicore::SlotContainer slots;
};
