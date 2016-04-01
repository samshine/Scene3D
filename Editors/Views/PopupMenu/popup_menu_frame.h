
#pragma once

class PopupMenuItemView : public uicore::RowView
{
public:
	PopupMenuItemView()
	{
		style()->set(R"(
			flex: none;
			padding: 3px 10px;
			)");

		label = add_child<uicore::LabelBaseView>();
	}

	void set_text(const std::string text)
	{
		label->set_text(uicore::Text::to_upper(text));
	}

	std::shared_ptr<uicore::LabelBaseView> label;
};

class PopupMenuFrameView : public uicore::ColumnView
{
public:
	PopupMenuFrameView()
	{
		style()->set(R"(
			width: 175px;
			background: rgb(103,103,103);
			font: 12px/18px 'Lato';
			color: rgb(230,230,230);
			/*margin: 0 10px 10px 0;
			box-shadow: 5px 5px 5px rgba(0,0,0,0.2);*/
			)");
	}
};
