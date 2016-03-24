
#pragma once

#include "Scene3D/SceneEngine/Passes/scene_pass.h"
#include "Scene3D/SceneEngine/resource.h"
#include <unordered_map>

class SceneImpl;
class SceneDecalImpl;
class SceneRender;

struct DecalRenderUniforms
{
	uicore::Mat4f eye_to_projection;
	float rcp_f = 0.0f;
	float rcp_f_div_aspect = 0.0f;
	uicore::Vec2f two_rcp_viewport_size;
};

class DecalsPass : public ScenePass
{
public:
	DecalsPass(SceneRender &inout);
	std::string name() const override { return "decals"; }
	void run() override;

private:
	void setup();

	struct InstanceBatch
	{
		std::string diffuse_texture;
		int start = 0;
		int count = 0;
		SceneDecalImpl **decals = nullptr;
	};

	void render_decals();
	void render_batches(const std::vector<InstanceBatch> &batches);
	void write_instance_data(const std::string &diffuse_texture, SceneDecalImpl *decal, uicore::Vec4f *vectors);
	void render_cube(const std::string &diffuse_texture, uicore::Texture2DPtr instance_buffer, int instance_base, int instance_count);

	uicore::Texture2DPtr get_instance_buffer(int size);
	uicore::StagingTexturePtr get_staging_buffer(int size);
	uicore::UniformVector<DecalRenderUniforms> get_uniform_buffer();

	const int vectors_per_instance = 4;
	const int num_vectors = 4 * 1024;

	SceneRender &inout;
	std::unordered_map<std::string, std::vector<SceneDecalImpl *>> render_lists;

	uicore::BlendStatePtr blend_state;
	uicore::DepthStencilStatePtr depth_stencil_state;
	uicore::RasterizerStatePtr rasterizer_state;
	uicore::ProgramObjectPtr program;

	uicore::PrimitivesArrayPtr prim_array;
	uicore::VertexArrayVector<uicore::Vec3f> box_positions;

	std::map<std::string, Resource<uicore::TexturePtr>> diffuse_textures;
};
