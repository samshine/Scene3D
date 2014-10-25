
#pragma once

class HeaderView : public clan::View
{
public:
	HeaderView();

private:
	void on_load();
	void on_save();
	void on_save_as();
	void on_options();

	void create_button(const std::string &text, std::function<void()> click);
};
