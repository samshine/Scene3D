
#pragma once

#include "Scene3D/SceneEngine/Passes/scene_pass.h"
#include "Scene3D/scene_particle_emitter.h"
#include "particle_emitter_pass_data.h"
#include "Scene3D/Scene/scene_particle_emitter_impl.h"
#include "Scene3D/SceneEngine/scene_render.h"
#include "Scene3D/SceneEngine/resource.h"

class SceneImpl;
class SceneEngineImpl;
class MaterialCache;

class ParticleEmitterPass : public ScenePass
{
public:
	ParticleEmitterPass(SceneRender &render);
	std::string name() const override { return "particle"; }
	void run() override;
	void update() override;

private:
	void select_active_emitters(const uicore::FrustumPlanes &frustum);
	void setup();

	SceneRender &inout;

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
