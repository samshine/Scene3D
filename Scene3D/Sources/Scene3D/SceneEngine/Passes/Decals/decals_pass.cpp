
#include "precomp.h"
#include "decals_pass.h"
#include "Scene3D/scene.h"
#include "Scene3D/Scene/scene_impl.h"
#include "Scene3D/Performance/scope_timer.h"
#include "vertex_decals_glsl.h"
#include "vertex_decals_hlsl.h"
#include "fragment_decals_glsl.h"
#include "fragment_decals_hlsl.h"

using namespace uicore;

DecalsPass::DecalsPass(SceneRender &inout) : inout(inout)
{
}

void DecalsPass::run()
{
	ScopeTimeFunction();

	// To do: implement http://www.slideshare.net/blindrenderer/screen-space-decals-in-warhammer-40000-space-marine-14699854
}
