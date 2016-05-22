
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

		auto item1 = std::make_shared<TreeItem>("Item 1");
		auto item2 = std::make_shared<TreeItem>("Item 2");
		tree_view->root_item()->add_child(item1);
		tree_view->root_item()->add_child(item2);

		auto child_item1 = item1->add_child(std::make_shared<TreeItem>("Child Item 1"));
		auto child_item2 = item1->add_child(std::make_shared<TreeItem>("Child Item 2"));
		auto child_item3 = item1->add_child(std::make_shared<TreeItem>("Child Item 3"));
		auto child_item4 = item2->add_child(std::make_shared<TreeItem>("Child Item 4"));
		auto child_item5 = item2->add_child(std::make_shared<TreeItem>("Child Item 5"));

		child_item1->add_child(std::make_shared<TreeItem>("Foobar"));
		child_item1->add_child(std::make_shared<TreeItem>("Lazy fox"));
		child_item1->add_child(std::make_shared<TreeItem>("Quick dog"));
		child_item1->add_child(std::make_shared<TreeItem>("Jumper"));
	}

	std::shared_ptr<TreeBaseView> tree_view;
};
