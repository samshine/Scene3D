
#pragma once

class TreeNodeView : public uicore::RowView
{
public:
	std::shared_ptr<uicore::ButtonBaseView> opener;
	std::shared_ptr<uicore::LabelBaseView> label;
};
