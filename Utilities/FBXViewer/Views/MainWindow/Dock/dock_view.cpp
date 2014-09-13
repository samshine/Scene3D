
#include "dock_view.h"
#include "AnimationsDockable/animations_dockable.h"

using namespace uicore;

DockView::DockView()
{
	style.set_layout_block();
	style.set_width(250.0f);
	style.set_flex(0.0f, 0.0f);
	style.set_background(Colorf::rgb8(240, 240, 240));

	animations = std::make_shared<AnimationsDockable>();
	add_subview(animations);
}
