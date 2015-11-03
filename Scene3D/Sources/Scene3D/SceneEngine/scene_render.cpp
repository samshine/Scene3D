
#include "precomp.h"
#include "scene_render.h"

using namespace uicore;

SceneRender::SceneRender(const uicore::GraphicContextPtr &gc, SceneEngineImpl *engine)
{
	model_shader_cache = std::make_unique<ModelShaderCache>();

	bool use_compute_shader_pass = true;

	if (gc->shader_language() == shader_glsl) // Compute shaders introduced in OpenGL 4.3
	{
		use_compute_shader_pass = gc->has_compute_shader_support();
	}
	else if (gc->shader_language() == shader_hlsl) // We need compute shaders of at least Direct3D feature level 11
	{
		use_compute_shader_pass = gc->major_version() > 10;
	}

	// use_compute_shader_pass = false; // Disable because it crashes with Oculus Rift

	passes.push_back(std::make_shared<GBufferPass>(*this));
	passes.push_back(std::make_shared<SkyboxPass>(*this));
	passes.push_back(std::make_shared<VSMShadowMapPass>(gc, *this));

	if (use_compute_shader_pass)
	{
		passes.push_back(std::make_shared<LightsourcePass>(gc, *this));
	}
	else
	{
		passes.push_back(std::make_shared<LightsourceSimplePass>(gc, *this));
	}

	passes.push_back(std::make_shared<TransparencyPass>(*this));
	passes.push_back(std::make_shared<ParticleEmitterPass>(engine));
	passes.push_back(std::make_shared<LensFlarePass>(*this));
	passes.push_back(std::make_shared<BloomPass>(gc, *this));
	//passes.push_back(std::make_shared<SSAOPass>(gc, *this));
	passes.push_back(std::make_shared<FinalPass>(gc, *this));
}

void SceneRender::setup_pass_buffers(const GraphicContextPtr &gc)
{
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
