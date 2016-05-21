
#pragma once

class TreeNodeView : public uicore::RowView
{
public:
	TreeNodeView()
	{
		opener = add_child<uicore::ButtonBaseView>();
		label = add_child<uicore::LabelBaseView>();
	}

	void set_ident(float indentation)
	{
		style()->set("margin-left: %1px", indentation);
	}

	std::shared_ptr<uicore::ButtonBaseView> opener;
	std::shared_ptr<uicore::LabelBaseView> label;
};
