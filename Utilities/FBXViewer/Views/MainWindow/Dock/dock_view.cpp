
#include "precomp.h"
#include "dock_view.h"
#include "AnimationsDockable/animations_dockable.h"

using namespace clan;

DockView::DockView()
{
	style.set_layout_block();
	style.set_width(250.0f);
	style.set_flex(0.0f, 0.0f);
	style.set_background(Colorf(47, 75, 99));
	style.set_border(Colorf(109, 109, 109), 1.0f);

	animations = std::make_shared<AnimationsDockable>();
	add_subview(animations);
}
