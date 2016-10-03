/*
	Original Lens Distortion Algorithm from SSontech
	http://www.ssontech.com/content/lensalg.htm

	If (u,v) are the coordinates of a feature in the undistorted perfect
	image plane, then (u', v') are the coordinates of the feature on the
	distorted image plate, ie the scanned or captured image from the
	camera. The distortion occurs radially away from the image center,
	with correction for the image aspect ratio (image_aspect = physical
	image width/height), as follows:

	r2 = image_aspect*image_aspect*u*u + v*v
	f = 1 + r2*(k + kcube*sqrt(r2))
	u' = f*u
	v' = f*v

	The constant k is the distortion coefficient that appears on the lens
	panel and through Sizzle. It is generally a small positive or negative
	number under 1%. The constant kcube is the cubic distortion value found
	on the image preprocessor's lens panel: it can be used to undistort or
	redistort images, but it does not affect or get computed by the solver.
	When no cubic distortion is needed, neither is the square root, saving
	time.

	Chromatic Aberration example,
	using red distord channel with green and blue undistord channel:

	k = vec3(-0.15, 0.0, 0.0);
	kcube = vec3(0.15, 0.0, 0.0);
*/

#include "precomp.h"
#include "lens_distortion_pass.h"
#include "Scene3D/SceneEngine/shader_setup.h"
#include "Scene3D/SceneEngine/vertex_screen_quad_glsl.h"
#include "Scene3D/SceneEngine/vertex_screen_quad_hlsl.h"
#include "Scene3D/Performance/scope_timer.h"
#include "fragment_lens_distortion_glsl.h"
#include "fragment_lens_distortion_hlsl.h"

using namespace uicore;

LensDistortionPass::LensDistortionPass(SceneRender &inout) : inout(inout)
{
	if (inout.gc->shader_language() == shader_glsl)
	{
		shader = ShaderSetup::compile(inout.gc, "lensdistort", vertex_screen_quad_glsl(), fragment_lens_distortion_glsl(), "");
		shader->bind_frag_data_location(0, "FragColor");
	}
	else
	{
		shader = ShaderSetup::compile(inout.gc, "lensdistort", vertex_screen_quad_hlsl(), fragment_lens_distortion_hlsl(), "");
	}

	shader->bind_attribute_location(0, "PositionInProjection");
	shader->bind_frag_data_location(0, "FragColor");

	ShaderSetup::link(shader, "lensdistort program");

	shader->set_uniform_buffer_index("Uniforms", 0);
	shader->set_uniform1i("InputTexture", 0);
	shader->set_uniform1i("InputTextureSampler", 0);

	uniforms = UniformVector<UniformBuffer>(inout.gc, 1);
}

void LensDistortionPass::run()
{
	ScopeTimeFunction();

	float lens_k = -0.12f;
	float lens_kcube = 0.1f;
	float lens_chromatic = 1.12f;

	Vec4f k(
		lens_k,
		lens_k * lens_chromatic,
		lens_k * lens_chromatic * lens_chromatic,
		0.0f);

	Vec4f kcube(
		lens_kcube,
		lens_kcube * lens_chromatic,
		lens_kcube * lens_chromatic * lens_chromatic,
		0.0f);

	Size viewport_size = inout.viewport_size;
	float aspect = viewport_size.width / (float)viewport_size.height;

	// Scale factor to keep sampling within the input texture
	float r2 = aspect * aspect * 0.25 + 0.25f;
	float sqrt_r2 = sqrt(r2);
	float f0 = 1.0f + std::max(r2 * (k.x + kcube.x * sqrt_r2), 0.0f);
	float f2 = 1.0f + std::max(r2 * (k.z + kcube.z * sqrt_r2), 0.0f);
	float f = std::max(f0, f2);
	float scale = 1.0f / f;

	UniformBuffer cpu_uniforms;
	cpu_uniforms.aspect = aspect;
	cpu_uniforms.scale = scale;
	cpu_uniforms.k = k;
	cpu_uniforms.kcube = kcube;
	uniforms.upload_data(inout.gc, &cpu_uniforms, 1);

	inout.frames.front()->current_pipeline_texture()->set_min_filter(filter_linear);
	inout.frames.front()->current_pipeline_texture()->set_mag_filter(filter_linear);

	inout.gc->set_frame_buffer(inout.frames.front()->next_pipeline_fb());
	inout.gc->set_viewport(inout.final_viewport, inout.gc->texture_image_y_axis());
	inout.gc->set_uniform_buffer(0, uniforms);
	inout.gc->set_texture(0, inout.frames.front()->current_pipeline_texture());
	inout.gc->set_program_object(shader);
	inout.gc->set_blend_state(inout.no_blend);
	inout.gc->set_rasterizer_state(inout.no_cull_rasterizer);
	inout.gc->draw_primitives(type_triangles, 6, inout.screen_quad_primitives);
	inout.gc->reset_program_object();
	inout.gc->reset_texture(0);
	inout.gc->reset_uniform_buffer(0);
	inout.gc->reset_frame_buffer();

	inout.frames.front()->next_pipeline_buffer();
}
