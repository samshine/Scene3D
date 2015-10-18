
#pragma once

#include "resource_object.h"
#include "Scene3D/Passes/GaussianBlur/gaussian_blur.h"
#include <memory>
#include <map>

class ResourceContainer
{
public:
	ResourceContainer() { }
	ResourceContainer(const ResourceContainer &) = delete;
	ResourceContainer &operator=(const ResourceContainer &) = delete;

	void setup_pass_buffers(const uicore::GraphicContextPtr &gc);

	uicore::Rect viewport = uicore::Size(640, 480);
	uicore::FrameBufferPtr fb_viewport;

	float field_of_view = 60.0f;
	uicore::Mat4f world_to_eye;

	uicore::FrameBufferPtr fb_gbuffer;
	uicore::FrameBufferPtr fb_self_illumination;
	uicore::FrameBufferPtr fb_bloom_extract;
	uicore::FrameBufferPtr fb_ambient_occlusion;
	uicore::FrameBufferPtr fb_final_color;

	uicore::Texture2DPtr zbuffer;

	uicore::Texture2DPtr diffuse_color_gbuffer;
	uicore::Texture2DPtr specular_color_gbuffer;
	uicore::Texture2DPtr specular_level_gbuffer;
	uicore::Texture2DPtr self_illumination_gbuffer;
	uicore::Texture2DPtr normal_z_gbuffer;

	uicore::Texture2DPtr bloom_contribution;
	uicore::Texture2DPtr ambient_occlusion;
	uicore::Texture2DPtr final_color;

	uicore::Texture2DArrayPtr shadow_maps;

	GaussianBlur blur;

	uicore::Texture2DPtr skybox_texture;
	bool show_skybox_stars = false;
};

inline void ResourceContainer::setup_pass_buffers(const uicore::GraphicContextPtr &gc)
{
	using namespace uicore;

	Size viewport_size = viewport.size();

	if (diffuse_color_gbuffer && diffuse_color_gbuffer->size() == viewport_size && gc->is_frame_buffer_owner(fb_gbuffer))
		return;

	fb_gbuffer = nullptr;
	fb_self_illumination = nullptr;
	fb_bloom_extract = nullptr;
	fb_ambient_occlusion = nullptr;
	fb_final_color = nullptr;
	zbuffer = nullptr;
	diffuse_color_gbuffer = nullptr;
	specular_color_gbuffer = nullptr;
	specular_level_gbuffer = nullptr;
	self_illumination_gbuffer = nullptr;
	normal_z_gbuffer = nullptr;
	bloom_contribution = nullptr;
	ambient_occlusion = nullptr;
	final_color = nullptr;

	gc->flush();

	diffuse_color_gbuffer = Texture2D::create(gc, viewport_size.width, viewport_size.height, tf_rgba16);
	specular_color_gbuffer = Texture2D::create(gc, viewport_size.width, viewport_size.height, tf_rgba8);
	specular_level_gbuffer = Texture2D::create(gc, viewport_size.width, viewport_size.height, tf_rg16f);
	self_illumination_gbuffer = Texture2D::create(gc, viewport_size.width, viewport_size.height, tf_rgba16);
	normal_z_gbuffer = Texture2D::create(gc, viewport_size.width, viewport_size.height, tf_rgba16f);
	zbuffer = Texture2D::create(gc, viewport_size.width, viewport_size.height, tf_depth_component24);

	final_color = Texture2D::create(gc, viewport_size.width, viewport_size.height, tf_rgba16f);

	Size bloom_size = viewport_size / 2;
	bloom_contribution = Texture2D::create(gc, bloom_size.width, bloom_size.height, tf_rgba8);

	ambient_occlusion = Texture2D::create(gc, viewport_size.width / 2, viewport_size.height / 2, tf_r8);
	ambient_occlusion->set_min_filter(filter_linear);
	ambient_occlusion->set_mag_filter(filter_linear);

	fb_gbuffer = FrameBuffer::create(gc);
	fb_gbuffer->attach_color(0, diffuse_color_gbuffer);
	fb_gbuffer->attach_color(1, specular_color_gbuffer);
	fb_gbuffer->attach_color(2, specular_level_gbuffer);
	fb_gbuffer->attach_color(3, self_illumination_gbuffer);
	fb_gbuffer->attach_color(4, normal_z_gbuffer);
	fb_gbuffer->attach_depth(zbuffer);

	fb_self_illumination = FrameBuffer::create(gc);
	fb_self_illumination->attach_color(0, self_illumination_gbuffer);
	fb_self_illumination->attach_depth(zbuffer);

	fb_final_color = FrameBuffer::create(gc);
	fb_final_color->attach_color(0, final_color);
	fb_final_color->attach_depth(zbuffer);

	fb_bloom_extract = FrameBuffer::create(gc);
	fb_bloom_extract->attach_color(0, bloom_contribution);

	fb_ambient_occlusion = FrameBuffer::create(gc);
	fb_ambient_occlusion->attach_color(0, ambient_occlusion);
}
