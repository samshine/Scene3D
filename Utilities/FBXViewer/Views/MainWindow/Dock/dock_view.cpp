
#include "precomp.h"
#include "dock_view.h"
#include "AnimationsDockable/animations_dockable.h"

using namespace clan;

DockView::DockView()
{
	box_style.set_layout_block();
	box_style.set_width(250.0f);
	box_style.set_flex(0.0f, 0.0f);
	box_style.set_background(Colorf(47, 75, 99));
	box_style.set_border(Colorf(109, 109, 109), 1.0f, 0.0f, 0.0f, 0.0f);

	animations = std::make_shared<AnimationsDockable>();
	add_subview(animations);
}
