
#include "precomp.h"
#include "diffuse_gi_pass_cs.h"
#include "Scene3D/Performance/scope_timer.h"
#include "Scene3D/Scene/scene_impl.h"
#include "accumulate_lpv_hlsl.h"
#include "init_gv_hlsl.h"
#include "init_lpv_hlsl.h"
#include "propagate_lpv_hlsl.h"
#include "render_result_hlsl.h"

using namespace uicore;

DiffuseGIPassCS::DiffuseGIPassCS(const GraphicContextPtr &gc, SceneRender &inout) : inout(inout)
{
	if (gc->shader_language() == shader_glsl)
	{
		//init_lpv_program = compile_and_link(gc, "init lpv", init_lpv_glsl());
		//init_gv_program = compile_and_link(gc, "init gv", init_gv_glsl());
		//propagate_lpv_program = compile_and_link(gc, "propagate lpv", propagate_lpv_glsl());
		//accumulate_lpv_program = compile_and_link(gc, "accumulate lpv", accumulate_lpv_glsl());
		//render_result_program = compile_and_link(gc, "render result", render_result_glsl());
	}
	else
	{
		init_lpv_program = compile_and_link(gc, "init lpv", init_lpv_hlsl());
		init_gv_program = compile_and_link(gc, "init gv", init_gv_hlsl());
		propagate_lpv_program = compile_and_link(gc, "propagate lpv", propagate_lpv_hlsl());
		accumulate_lpv_program = compile_and_link(gc, "accumulate lpv", accumulate_lpv_hlsl());
		render_result_program = compile_and_link(gc, "render result", render_result_hlsl());
	}
}

DiffuseGIPassCS::~DiffuseGIPassCS()
{
}

void DiffuseGIPassCS::run()
{
	ScopeTimeFunction();
/*
	update_buffers(gc);

	Size viewport = diffuse_color_gbuffer->size();
	int tile_size = 16;
	int num_tiles_x = (viewport.width + tile_size - 1) / tile_size;
	int num_tiles_y = (viewport.height + tile_size - 1) / tile_size;

	//ComputeShaderDispatch init_lpv(init_lpv_kernel);
	//init_lpv.bind("shadow_maps", shadow_maps_srv);
	//init_lpv.dispatch(gc, x, y);

	ComputeShaderDispatch render_result(render_result_kernel);
	render_result.bind("normal_z", normal_z_srv);
	render_result.bind("diffuse", diffuse_srv);
	render_result.bind("in_final", final_srv);
	render_result.bind("out_final", final_uav);
	render_result.dispatch(gc, num_tiles_x, num_tiles_y);
*/
}

void DiffuseGIPassCS::update_buffers(const GraphicContextPtr &gc)
{
	ScopeTimeFunction();
}

ProgramObjectPtr DiffuseGIPassCS::compile_and_link(const GraphicContextPtr &gc, const std::string &program_name, const std::string &source)
{
	auto compute_shader = ShaderObject::create(gc, ShaderType::compute, source);
	if (!compute_shader->try_compile())
		throw Exception(string_format("Unable to compile %1 compute shader: %2", program_name, compute_shader->info_log()));

	auto program = ProgramObject::create(gc);
	program->attach(compute_shader);
	if (!program->try_link())
		throw Exception(string_format("Failed to link %1: %2", program_name, program->info_log()));

	// Uniforms
	//program.set_uniform_buffer_index("Uniforms", 0);

	// Storage buffers
	//program.set_storage_buffer_index("Lights", 0);

	// Textures
	//program.set_uniform1i("zminmax", 0);

	// Samplers
	//program.set_uniform1i("shadow_maps_sampler", 5);

	// Images
	//program.set_uniform1i("out_final", 0);

	return program;
}
