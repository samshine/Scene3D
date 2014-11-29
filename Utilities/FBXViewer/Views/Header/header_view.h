
#pragma once

class HeaderView : public clan::View
{
public:
	HeaderView();

	void add_left_button(const std::string &text, std::function<void()> click);
	void add_right_button(const std::string &text, std::function<void()> click);

private:
	void create_button(const std::string &text, std::function<void()> click, bool left);

	std::shared_ptr<View> left_buttons;
	std::shared_ptr<View> right_buttons;
};
