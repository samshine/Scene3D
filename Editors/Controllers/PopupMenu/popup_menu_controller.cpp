
#include "precomp.h"
#include "popup_menu_controller.h"

using namespace uicore;

PopupMenuController::PopupMenuController(const std::vector<PopupMenuItem> &items)
{
	set_root_view(view);

	for (const auto &item : items)
	{
		if (item.custom_view)
		{
			view->add_child(item.custom_view);
		}
		else
		{
			auto item_view = view->add_child<PopupMenuItemView>();
			item_view->set_text(item.label);

			auto clicked = item.clicked;
			slots.connect(item_view->sig_pointer_release(), [=](PointerEvent &e) {
				if (clicked)
					clicked();
				dismiss();
			});
		}
	}
}
