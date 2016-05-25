
#pragma once

#include "Views/AssetCompilerWindow/asset_compiler_window_view.h"
#include "build_controller.h"
#include "asset_explorer_controller.h"

class AssetCompilerWindow : public uicore::WindowController
{
public:
	AssetCompilerWindow();

private:
	void update_window_title();
	void on_open();

	std::shared_ptr<AssetCompilerWindowView> view = std::make_shared<AssetCompilerWindowView>();
	std::shared_ptr<AssetExplorerController> asset_explorer_controller = std::make_shared<AssetExplorerController>();
	std::shared_ptr<BuildController> build_controller = std::make_shared<BuildController>();
};
