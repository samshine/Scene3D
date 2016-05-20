
#pragma once

#include "Views/Theme/theme_views.h"

class AssetExplorerView : public uicore::ColumnView
{
public:
	AssetExplorerView()
	{
		using namespace uicore;

		style()->set(R"(
			margin: 11px;
			flex: auto;
			border: 1px solid #ddd;
			background: white;
			padding: 2px
		)");
	}
};
