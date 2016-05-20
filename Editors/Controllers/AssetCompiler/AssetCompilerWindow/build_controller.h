
#pragma once

#include "Views/AssetCompilerWindow/build_view.h"

class BuildController
{
public:
	BuildController();

	std::shared_ptr<BuildView> view = std::make_shared<BuildView>();

private:
	void project_folder_browse_clicked();
	void build_button_clicked();
	void clean_button_clicked();
	void cancel_button_clicked();

	void create_compiler();

	AssetCompilerPtr compiler;

	uicore::SlotContainer slots;
};
