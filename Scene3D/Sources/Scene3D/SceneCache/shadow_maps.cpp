
#include "precomp.h"
#include "shadow_maps.h"

using namespace uicore;

ShadowMaps::ShadowMaps(GraphicContext &gc, const Resource<Texture2DArray> &init_shadow_maps, int shadow_map_size, int max_active_maps, TextureFormat format)
: shadow_maps(init_shadow_maps), used_entries(0), unused_entries(0)
{
	framebuffers.reserve(max_active_maps);
	views.reserve(max_active_maps);
	shadow_maps.set(Texture2DArray(gc, shadow_map_size, shadow_map_size, max_active_maps, format));

	Texture2D depth_texture(gc, shadow_map_size, shadow_map_size, tf_depth_component32);
	for (int i = 0; i < max_active_maps; i++)
	{
		FrameBuffer fb(gc);
		fb.attach_color(0, shadow_maps.get(), i);
		fb.attach_depth(depth_texture);
		framebuffers.push_back(fb);

		views.push_back(shadow_maps->create_2d_view(i, tf_rg32f, 0, 1));

		free_indexes.push_back(i);
	}
}

ShadowMaps::~ShadowMaps()
{
}

void ShadowMaps::start_frame()
{
	// Move all entries to unused list:
	while (used_entries)
	{
		ShadowMapEntry_Impl *entry = used_entries;
		unlink(entry);
		add_unused(entry);
	}
}

void ShadowMaps::assign_indexes()
{
	// Assign an index to all used entries, or page them out if we run out of slots
	ShadowMapEntry_Impl *entry = used_entries;
	while (entry)
	{
		if (entry->index == -1)
		{
			if (!free_indexes.empty())
			{
				entry->index = free_indexes.back();
				free_indexes.pop_back();
			}
			else if (unused_entries)
			{
				entry->index = unused_entries->index;
				unused_entries->index = -1;
				unlink(unused_entries);
			}
		}

		ShadowMapEntry_Impl *next_entry = entry->next;
		if (entry->index == -1)
			unlink(entry);
		entry = next_entry;
	}
}

void ShadowMaps::add_used(ShadowMapEntry_Impl *entry)
{
	unlink(entry);
	if (used_entries)
		used_entries->prev = entry;
	entry->next = used_entries;
	used_entries = entry;
}

void ShadowMaps::add_unused(ShadowMapEntry_Impl *entry)
{
	unlink(entry);
	if (unused_entries)
		unused_entries->prev = entry;
	entry->next = unused_entries;
	unused_entries = entry;
}

void ShadowMaps::unlink(ShadowMapEntry_Impl *entry)
{
	if (used_entries == entry)
		used_entries = entry->next;
	else if (unused_entries == entry)
		unused_entries = entry->next;

	if (entry->prev)
		entry->prev->next = entry->next;
	if (entry->next)
		entry->next->prev = entry->prev;

	entry->prev = 0;
	entry->next = 0;
}

void ShadowMaps::use_entry(ShadowMapEntry_Impl *entry)
{
	unlink(entry);
	add_used(entry);
}

void ShadowMaps::entry_destroyed(ShadowMapEntry_Impl *entry)
{
	unlink(entry);
	if (entry->index != -1)
	{
		free_indexes.push_back(entry->index);
		entry->index = -1;
	}
}

/////////////////////////////////////////////////////////////////////////////

ShadowMapEntry::ShadowMapEntry()
{
}

ShadowMapEntry::ShadowMapEntry(ShadowMaps *shadow_maps)
: impl(std::make_shared<ShadowMapEntry_Impl>(shadow_maps))
{
}

void ShadowMapEntry::use_in_frame()
{
	impl->shadow_maps->use_entry(impl.get());
}

int ShadowMapEntry::get_index() const
{
	return impl->index;
}

FrameBuffer ShadowMapEntry::get_framebuffer() const
{
	if (impl->index != -1)
		return impl->shadow_maps->framebuffers[impl->index];
	else
		return FrameBuffer();
}

Texture2D ShadowMapEntry::get_view() const
{
	if (impl->index != -1)
		return impl->shadow_maps->views[impl->index];
	else
		return Texture2D();
}
