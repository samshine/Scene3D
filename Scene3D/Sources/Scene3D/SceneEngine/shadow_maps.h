
#pragma once

class SceneLightImpl;
class SceneRender;

class ShadowMapLight
{
public:
	uicore::FrameBufferPtr framebuffer;
	uicore::Texture2DPtr view;

	SceneLightImpl *light = nullptr;
	std::weak_ptr<SceneLightImpl> light_weakptr; // Used to detect destroyed lights in start_frame

	float sqr_distance = 0.0f;
};

class ShadowMaps
{
public:
	ShadowMaps(SceneRender &render);

	void setup(int shadow_map_size, int max_active_maps, uicore::TextureFormat format);

	void start_frame();
	void add_light(SceneLightImpl *light);

	uicore::Texture2DArrayPtr shadow_maps;
	uicore::FrameBufferPtr fb_blur;
	uicore::Texture2DPtr blur_texture;
	std::vector<ShadowMapLight> lights;

private:
	SceneRender &render;
};
