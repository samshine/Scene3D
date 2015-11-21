
#include "precomp.h"
#include "theme_views.h"

ThemeScrollView::ThemeScrollView()
{
	scrollbar_x_view()->style()->set("flex: none");
	scrollbar_x_view()->track()->style()->set("padding: 0 4px");
	scrollbar_x_view()->thumb()->style()->set("background: rgb(208,209,215)");

	scrollbar_y_view()->style()->set("flex: none");
	scrollbar_y_view()->track()->style()->set("padding: 0 4px");
	scrollbar_y_view()->thumb()->style()->set("background: rgb(208,209,215)");
}

ThemeScrollBarView::ThemeScrollBarView()
{
	style()->set("flex: none");
	track()->style()->set("padding: 0 4px");
	thumb()->style()->set("background: rgb(208,209,215)");
}
