
#pragma once

#include <list>
#include "resource.h"

class ShadowMaps;
class ShadowMapEntry;
class ShadowMapEntry_Impl;

class ShadowMaps
{
public:
	ShadowMaps(uicore::GraphicContext &gc, const Resource<uicore::Texture2DArray> &shadow_maps, int shadow_map_size, int max_active_maps, uicore::TextureFormat format);
	~ShadowMaps();

	void start_frame();
	void assign_indexes();

private:
	void use_entry(ShadowMapEntry_Impl *entry);
	void entry_destroyed(ShadowMapEntry_Impl *entry);

	void add_used(ShadowMapEntry_Impl *entry);
	void add_unused(ShadowMapEntry_Impl *entry);
	void unlink(ShadowMapEntry_Impl *entry);

	Resource<uicore::Texture2DArray> shadow_maps;

	std::vector<uicore::FrameBuffer> framebuffers;
	std::vector<uicore::Texture2D> views;

	ShadowMapEntry_Impl *used_entries;
	ShadowMapEntry_Impl *unused_entries;
	std::vector<int> free_indexes;

	friend class ShadowMapEntry;
	friend class ShadowMapEntry_Impl;
};

class ShadowMapEntry
{
public:
	ShadowMapEntry();
	ShadowMapEntry(ShadowMaps *shadow_maps);
	bool is_null() const { return !impl; }

	void use_in_frame();

	int get_index() const;
	uicore::FrameBuffer get_framebuffer() const;
	uicore::Texture2D get_view() const;

private:
	std::shared_ptr<ShadowMapEntry_Impl> impl;
};

class ShadowMapEntry_Impl
{
public:
	ShadowMapEntry_Impl(ShadowMaps *maps) : shadow_maps(maps), index(-1), prev(0), next(0) { }
	~ShadowMapEntry_Impl() { shadow_maps->entry_destroyed(this); }

	ShadowMaps *shadow_maps;
	int index;

	ShadowMapEntry_Impl *prev;
	ShadowMapEntry_Impl *next;
};

