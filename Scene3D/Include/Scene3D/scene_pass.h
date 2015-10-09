
#pragma once

#include <memory>
#include <string>

class Scene;
class ScenePass_Impl;

class ScenePass
{
public:
	ScenePass();
	ScenePass(std::shared_ptr<ScenePass_Impl> impl);

	bool is_null() const;

	std::function<void(const uicore::GraphicContextPtr &)> &func_run();

	const std::string &get_name() const;

private:
	std::shared_ptr<ScenePass_Impl> impl;
};
