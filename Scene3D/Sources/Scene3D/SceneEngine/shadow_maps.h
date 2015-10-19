
#pragma once

#include <list>

class ShadowMaps;
class ShadowMapEntry;
class ShadowMapEntryImpl;
class SceneRender;

class ShadowMaps
{
public:
	ShadowMaps(const uicore::GraphicContextPtr &gc, SceneRender &render, int shadow_map_size, int max_active_maps, uicore::TextureFormat format);
	~ShadowMaps();

	void start_frame();
	void assign_indexes();

private:
	void use_entry(ShadowMapEntryImpl *entry);
	void entry_destroyed(ShadowMapEntryImpl *entry);

	void add_used(ShadowMapEntryImpl *entry);
	void add_unused(ShadowMapEntryImpl *entry);
	void unlink(ShadowMapEntryImpl *entry);

	SceneRender &render;

	std::vector<uicore::FrameBufferPtr> framebuffers;
	std::vector<uicore::Texture2DPtr> views;

	ShadowMapEntryImpl *used_entries;
	ShadowMapEntryImpl *unused_entries;
	std::vector<int> free_indexes;

	friend class ShadowMapEntry;
	friend class ShadowMapEntryImpl;
};

class ShadowMapEntry
{
public:
	ShadowMapEntry();
	ShadowMapEntry(ShadowMaps *shadow_maps);
	bool is_null() const { return !impl; }

	void use_in_frame();

	int get_index() const;
	uicore::FrameBufferPtr get_framebuffer() const;
	uicore::Texture2DPtr get_view() const;

private:
	std::shared_ptr<ShadowMapEntryImpl> impl;
};

class ShadowMapEntryImpl
{
public:
	ShadowMapEntryImpl(ShadowMaps *maps) : shadow_maps(maps), index(-1), prev(0), next(0) { }
	~ShadowMapEntryImpl() { shadow_maps->entry_destroyed(this); }

	ShadowMaps *shadow_maps;
	int index;

	ShadowMapEntryImpl *prev;
	ShadowMapEntryImpl *next;
};

