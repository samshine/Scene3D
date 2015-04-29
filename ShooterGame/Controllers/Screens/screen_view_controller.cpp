
#include "precomp.h"
#include "screen_view_controller.h"

using namespace clan;

ScreenViewController::ScreenViewController(Canvas &canvas)
{
	view = std::make_shared<TextureView>(canvas);
	texture_view()->set_viewport(canvas.get_size());
	texture_view()->set_clear_background(false);
}

std::shared_ptr<TextureView> ScreenViewController::texture_view()
{
	return std::dynamic_pointer_cast<TextureView>(view);
}

/////////////////////////////////////////////////////////////////////////////

void Screen::set(const std::shared_ptr<ScreenViewController> &new_active)
{
	active = new_active;
}

std::shared_ptr<ScreenViewController> Screen::get()
{
	return active;
}

std::shared_ptr<ScreenViewController> Screen::active;
