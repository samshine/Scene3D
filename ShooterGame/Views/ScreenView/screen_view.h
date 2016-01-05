
#pragma once

class ScreenView
{
public:
	virtual void update() { }

	const uicore::DisplayWindowPtr &window() const;
	uicore::Vec2i mouse_delta() const;
	const uicore::GameTime game_time() const;

	const uicore::GraphicContextPtr &gc() const;
	const uicore::CanvasPtr &canvas() const;

	const SceneEnginePtr &scene_engine() const;
	const SceneViewportPtr &scene_viewport() const;

	const std::shared_ptr<SoundCache> &sound_cache() const;

protected:
	uicore::SlotContainer slots;
};
