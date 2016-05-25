
#pragma once

#include "Views/Theme/theme_views.h"
#include "tree_view.h"
#include "tree_item.h"

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

		tree_view = add_child<TreeBaseView>();

		tree_view->scrollbar_x_view()->style()->set("flex: none");
		tree_view->scrollbar_x_view()->track()->style()->set("padding: 0 4px");
		tree_view->scrollbar_x_view()->thumb()->style()->set("background: rgb(208,209,215)");

		tree_view->scrollbar_y_view()->style()->set("flex: none");
		tree_view->scrollbar_y_view()->track()->style()->set("padding: 0 4px");
		tree_view->scrollbar_y_view()->thumb()->style()->set("background: rgb(208,209,215)");

	}

	std::shared_ptr<TreeBaseView> tree_view;
};
