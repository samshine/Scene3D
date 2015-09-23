
#pragma once

class HeaderMenuView : public uicore::View
{
public:
	HeaderMenuView(const std::string &text, const std::string &icon, bool last);

	void add_item(const std::string &text, std::function<void()> click);

private:
	void button_clicked(uicore::PointerEvent &e);

	std::shared_ptr<uicore::ButtonView> button;
	std::shared_ptr<uicore::PopupView> items;
};
