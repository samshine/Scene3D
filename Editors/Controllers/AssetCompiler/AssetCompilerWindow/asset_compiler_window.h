
#pragma once

#include "Views/AssetCompilerWindow/asset_compiler_window_view.h"

class AssetCompilerWindow : public uicore::WindowController
{
public:
	AssetCompilerWindow();

private:
	void asset_list_browse_clicked();
	void output_browse_clicked();
	void build_button_clicked();
	void clean_button_clicked();
	void cancel_button_clicked();

	void create_compiler();

	AssetCompiler compiler;
	std::shared_ptr<AssetCompilerWindowView> view = std::make_shared<AssetCompilerWindowView>();

	uicore::SlotContainer slots;
};
