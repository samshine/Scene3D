
#pragma once

#include <physics3d.h>
#include <Collision3D/triangle_mesh_shape.h>
#include "AssetCompiler/MapDescription/map_desc.h"

class RayTracerLight
{
public:
	uicore::Vec3f position;
	uicore::Vec3f color;
	float attenuation_start = 0.0f;
	float attenuation_end = 0.0f;
};

class RayTracer
{
public:
	RayTracer(const MapDesc &desc);

	void generate_diffuse_gi(const std::string &filename);

private:
	uicore::Vec3f raytrace_diffuse_gi(const uicore::Vec3f &point) const;

	Physics3DWorldPtr world = Physics3DWorld::create();
	std::map<std::string, std::vector<RayTracerLight>> mesh_lights;
	std::map<std::string, Physics3DShapePtr> shapes;
	std::vector<Physics3DObjectPtr> objects;
	std::vector<RayTracerLight> lights;
};
