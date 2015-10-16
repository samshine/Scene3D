
#pragma once

#include "Scene3D/Passes/scene_pass.h"
#include "Scene3D/scene_particle_emitter.h"
#include "particle_emitter_pass_data.h"
#include "Scene3D/scene_particle_emitter_impl.h"
#include "Scene3D/SceneCache/resource_container.h"
#include "Scene3D/SceneCache/resource.h"

class SceneImpl;
class MaterialCache;

class ParticleEmitterPass : public ScenePass
{
public:
	ParticleEmitterPass(MaterialCache &texture_cache, const std::string &shader_path, ResourceContainer &inout);
	std::string name() const override { return "particle"; }
	void run(const uicore::GraphicContextPtr &gc, SceneImpl *scene) override;
	void update(const uicore::GraphicContextPtr &gc, float time_elapsed) override;

private:
	void select_active_emitters(const uicore::GraphicContextPtr &gc, SceneImpl *scene, const uicore::FrustumPlanes &frustum);
	void setup(const uicore::GraphicContextPtr &gc);

	// In:
	Resource<uicore::Rect> viewport;
	Resource<float> field_of_view;
	Resource<uicore::Mat4f> world_to_eye;
	Resource<uicore::Texture2DPtr> zbuffer;
	Resource<uicore::Texture2DPtr> normal_z_gbuffer;

	// InOut:
	Resource<uicore::Texture2DPtr> final_color;

	std::string shader_path;

	MaterialCache &texture_cache;
	uicore::FrameBufferPtr fb;
	uicore::BlendStatePtr blend_state;
	uicore::DepthStencilStatePtr depth_stencil_state;
	uicore::RasterizerStatePtr rasterizer_state;
	uicore::ProgramObjectPtr program;

	uicore::PrimitivesArrayPtr prim_array;
	uicore::VertexArrayVector<uicore::Vec3f> billboard_positions;
	static uicore::Vec3f cpu_billboard_positions[6];

	uicore::Texture2DPtr instance_texture;
	uicore::StagingTexturePtr instance_transfer;

	std::vector<ParticleEmitterPassData> emitter_slots;
	std::vector<int> active_emitters;
	std::vector<int> free_emitters;

	struct ParticleOrderIndex
	{
		ParticleOrderIndex(int index, float sqr_distance) : index(index), sqr_distance(sqr_distance) { }

		bool operator<(const ParticleOrderIndex &that) const
		{
			return sqr_distance > that.sqr_distance;
		}

		int index;
		float sqr_distance;
	};
};
