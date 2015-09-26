
#pragma once

#include "Scene3D/scene_cull_provider.h"

class PortalSector;

class PortalClipping
{
public:
	PortalClipping(const uicore::FrustumPlanes &frustum, const uicore::Mat4f &world_to_projection);
	uicore::Rectf intersect(uicore::Rectf box, const std::vector<uicore::Vec3f> &points) const;

	uicore::Mat4f world_to_projection;
	uicore::Mat4f projection_to_world;
	uicore::FrustumPlanes frustum;

private:
	uicore::Rectf project(const std::vector<uicore::Vec2f> &points) const;
	void find_edges(const std::vector<uicore::Vec3f> &points, std::vector<uicore::Vec2f> &out_edges) const;
	void add_triangle_edges(const std::vector<uicore::Vec3f> &points, int v0, int v1, int v2, std::vector<uicore::Vec2f> &out_edges) const;
};
