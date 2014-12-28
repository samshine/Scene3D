
#pragma once

class HeaderMenuView;

class HeaderView : public clan::View
{
public:
	HeaderView();

	void add_left_button(const std::string &text, std::function<void()> click);
	void add_left_button(const std::string &text, const std::string &icon, std::function<void()> click);
	void add_right_button(const std::string &text, std::function<void()> click);
	void add_right_button(const std::string &text, const std::string &icon, std::function<void()> click, bool last = false);

	std::shared_ptr<HeaderMenuView> add_right_menu(const std::string &text, const std::string &icon, bool last = false);

private:
	void create_button(const std::string &text, const std::string &icon, std::function<void()> click, bool left, bool last = false);

	std::shared_ptr<clan::View> left_buttons;
	std::shared_ptr<clan::View> right_buttons;
};
