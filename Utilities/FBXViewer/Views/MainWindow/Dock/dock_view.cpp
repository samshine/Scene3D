
#include "precomp.h"
#include "dock_view.h"
#include "AnimationsDockable/animations_dockable.h"

using namespace clan;

DockView::DockView()
{
	style.set_layout_block();
	style.set_width(250.0f);
	style.set_flex(0.0f, 0.0f);
	style.set_background(Colorf(240, 240, 240));

	animations = std::make_shared<AnimationsDockable>();
	add_subview(animations);
}
