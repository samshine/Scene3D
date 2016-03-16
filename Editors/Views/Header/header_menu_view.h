
#pragma once

class HeaderMenuPopupController;

class HeaderMenuView : public uicore::View
{
public:
	HeaderMenuView(const std::string &text, const std::string &icon, bool last);

	void add_item(const std::string &text, std::function<void()> click);

private:
	void button_clicked(uicore::PointerEvent &e);

	std::shared_ptr<uicore::ButtonBaseView> button;
	std::shared_ptr<HeaderMenuPopupController> menu;
};

class HeaderMenuPopupController : public uicore::WindowController
{
public:
	HeaderMenuPopupController(HeaderMenuView *header_view, bool last)
	{
		//items->style()->set("top: 40px");
		items->style()->set("width: 175px");
		items->style()->set("background: rgb(15,50,77)");
		items->style()->set("margin: 0 10px 10px 0");
		items->style()->set("box-shadow: 5px 5px 5px rgba(0,0,0,0.2)");
		items->set_hidden(true);

		set_root_view(items);
	}

	std::shared_ptr<uicore::ColumnView> items = std::make_shared<uicore::ColumnView>();
};
