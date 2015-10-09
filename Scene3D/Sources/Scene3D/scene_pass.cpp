
#include "precomp.h"
#include "Scene3D/scene_pass.h"
#include "Scene3D/scene.h"
#include "scene_pass_impl.h"
#include "scene_impl.h"

using namespace uicore;

ScenePass::ScenePass()
{
}

ScenePass::ScenePass(std::shared_ptr<ScenePass_Impl> impl)
	: impl(impl)
{
}

bool ScenePass::is_null() const
{
	return !impl;
}

std::function<void(const GraphicContextPtr &)> &ScenePass::func_run()
{
	return impl->cb_run;
}

const std::string &ScenePass::get_name() const
{
	return impl->name;
}
