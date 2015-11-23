
#pragma once

class AssetCompilerWindow : public uicore::WindowController
{
public:
	AssetCompilerWindow();

private:
	std::shared_ptr<uicore::ColumnView> view = std::make_shared<uicore::ColumnView>();

	uicore::SlotContainer slots;
};
