
#pragma once

#include <vector>
#include "map_desc_light.h"
#include "map_desc_light_probe.h"
#include "map_desc_particle_emitter.h"
#include "map_desc_material.h"
#include "map_desc_object.h"
#include "map_desc_trigger.h"
#include "map_desc_path_node.h"

namespace uicore
{
	class MapDesc
	{
	public:
		MapDesc();

		std::string fbx_filename;
		std::vector<MapDescLight> lights;
		std::vector<MapDescLightProbe> light_probes;
		std::vector<MapDescParticleEmitter> emitters;
		std::vector<MapDescMaterial> materials;
		std::vector<MapDescObject> objects;
		std::vector<MapDescTrigger> triggers;
		std::vector<MapDescPathNode> path_nodes;

		static MapDesc load(const std::string &filename);
		void save(const std::string &filename);
	};
}
