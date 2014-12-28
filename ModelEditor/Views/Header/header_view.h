
#pragma once

class HeaderView : public clan::View
{
public:
	HeaderView();

	void add_left_button(const std::string &text, std::function<void()> click);
	void add_left_button(const std::string &text, const std::string &icon, std::function<void()> click);
	void add_right_button(const std::string &text, std::function<void()> click);
	void add_right_button(const std::string &text, const std::string &icon, std::function<void()> click, bool last = false);

private:
	void create_button(const std::string &text, const std::string &icon, std::function<void()> click, bool left, bool last = false);

	std::shared_ptr<View> left_buttons;
	std::shared_ptr<View> right_buttons;
};
