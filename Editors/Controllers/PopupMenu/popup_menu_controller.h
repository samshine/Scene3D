
#pragma once

#include "Views/PopupMenu/popup_menu_frame.h"

class PopupMenuFrameView;

class PopupMenuItem
{
public:
	PopupMenuItem() { }
	PopupMenuItem(const std::string &label) : label(label) { }
	PopupMenuItem(const std::string &label, const std::function<void()> &clicked) : label(label), clicked(clicked) { }
	PopupMenuItem(std::shared_ptr<uicore::View> custom_view) : custom_view(custom_view) { }

	std::string label;
	std::function<void()> clicked;
	std::shared_ptr<uicore::View> custom_view;
};

class PopupMenuController : public uicore::WindowController
{
public:
	PopupMenuController(const std::vector<PopupMenuItem> &items);

	std::shared_ptr<PopupMenuFrameView> view = std::make_shared<PopupMenuFrameView>();
};
