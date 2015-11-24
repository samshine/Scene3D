
#pragma once

#include <vector>
#include "map_desc_light.h"
#include "map_desc_light_probe.h"
#include "map_desc_particle_emitter.h"
#include "map_desc_material.h"
#include "map_desc_object.h"
#include "map_desc_trigger.h"
#include "map_desc_path_node.h"

class MapDesc
{
public:
	MapDesc();

	std::vector<MapDescLight> lights;
	std::vector<MapDescLightProbe> light_probes;
	std::vector<MapDescParticleEmitter> emitters;
	std::vector<MapDescMaterial> materials;
	std::vector<MapDescObject> objects;
	std::vector<MapDescTrigger> triggers;
	std::vector<MapDescPathNode> path_nodes;

	std::shared_ptr<MapData> convert();

	static MapDesc load(const std::string &filename);
	void save(const std::string &filename);
};
