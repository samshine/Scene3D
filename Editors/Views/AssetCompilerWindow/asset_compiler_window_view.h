
#pragma once

#include "Views/Header/header_view.h"

class AssetCompilerWindowView : public uicore::ColumnView
{
public:
	AssetCompilerWindowView()
	{
		using namespace uicore;

		header = add_child<HeaderBrightView>();
		auto row = add_child<RowView>();

		sidebar = row->add_child<ColumnView>();
		center = row->add_child<ColumnView>();

		style()->set(R"(
			background: rgb(240,240,240);
			font: 12px/15px 'Segoe UI';
		)");

		row->style()->set(R"(
			flex: auto;
			margin-top: -11px;
		)");

		sidebar->style()->set(R"(
			width: 300px;
		)");

		center->style()->set(R"(
			flex: 1;
		)");
	}

	std::shared_ptr<HeaderView> header;
	std::shared_ptr<uicore::ColumnView> sidebar;
	std::shared_ptr<uicore::ColumnView> center;
};
