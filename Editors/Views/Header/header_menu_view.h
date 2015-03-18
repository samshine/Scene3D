
#pragma once

class HeaderMenuView : public clan::View
{
public:
	HeaderMenuView(const std::string &text, const std::string &icon, bool last);

	void add_item(const std::string &text, std::function<void()> click);

private:
	void button_clicked(clan::PointerEvent &e);

	std::shared_ptr<clan::ButtonView> button;
	std::shared_ptr<clan::PopupView> items;
};
