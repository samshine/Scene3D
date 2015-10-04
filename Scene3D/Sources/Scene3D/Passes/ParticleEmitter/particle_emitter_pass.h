
#pragma once

#include "Scene3D/scene_particle_emitter.h"
#include "particle_emitter_pass_data.h"
#include "Scene3D/scene_particle_emitter_impl.h"
#include "Scene3D/SceneCache/resource_container.h"
#include "Scene3D/SceneCache/resource.h"

class Scene_Impl;
class MaterialCache;

class ParticleEmitterPass : SceneParticleEmitterVisitor
{
public:
	ParticleEmitterPass(MaterialCache &texture_cache, const std::string &shader_path, ResourceContainer &inout);
	void run(uicore::GraphicContext &gc, Scene_Impl *scene);
	void update(uicore::GraphicContext &gc, float time_elapsed);

private:
	void setup(uicore::GraphicContext &gc);
	void emitter(uicore::GraphicContext &gc, const uicore::Mat4f &world_to_eye, const uicore::Mat4f &eye_to_projection, SceneParticleEmitter_Impl *emitter);

	// In:
	Resource<uicore::Rect> viewport;
	Resource<float> field_of_view;
	Resource<uicore::Mat4f> world_to_eye;
	Resource<uicore::Texture2D> zbuffer;
	Resource<uicore::Texture2D> normal_z_gbuffer;

	// InOut:
	Resource<uicore::Texture2D> final_color;

	std::string shader_path;

	MaterialCache &texture_cache;
	uicore::FrameBuffer fb;
	uicore::BlendStatePtr blend_state;
	uicore::DepthStencilStatePtr depth_stencil_state;
	uicore::RasterizerStatePtr rasterizer_state;
	uicore::ProgramObjectPtr program;

	uicore::PrimitivesArray prim_array;
	uicore::VertexArrayVector<uicore::Vec3f> billboard_positions;
	static uicore::Vec3f cpu_billboard_positions[6];

	uicore::Texture2D instance_texture;
	uicore::TransferTexture instance_transfer;

	std::vector< std::shared_ptr<ParticleEmitterPassData> > active_emitters;

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
