
#include "precomp.h"
#include "texture_atlas.h"

using namespace uicore;

TextureAtlas::TextureAtlas(Size texture_size, bool search_previous_textures) : initial_texture_size(texture_size), search_previous_textures(search_previous_textures)
{
}

TextureAtlasObject TextureAtlas::add(Size texture_size)
{
	// Try inserting in current active texture
	int array_index = (int)roots.size() - 1;

	TextureAtlasNode *node = 0;
	if (!roots.empty())
	{
		node = roots.back()->insert(texture_size, next_id);
	}

	if (!node) // Couldn't find a fit in current active texture
	{
		if (search_previous_textures)
		{
			array_index = 0;
			for (auto &root : roots)
			{
				node = root->insert(texture_size, next_id);
				if (node)	// We found space in a previous texture
					break;
				array_index++;
			}
		}

		if (!node) // Couldn't find a fit, so create a new texture
		{
			if (texture_size.width > initial_texture_size.width || texture_size.height > initial_texture_size.height)
			{
				// If the specified size is greater than the initial size,  then create a texture using the specified size
				add_new_root(texture_size);
			}
			else
			{
				add_new_root(initial_texture_size);
			}

			node = roots.back()->insert(texture_size, next_id);
			array_index = (int)roots.size() - 1;
		}

		if (!node)
			throw Exception("Unable to pack rect into texture atlas");
	}

	next_id++;

	return TextureAtlasObject(array_index, node->image_rect);
}

int TextureAtlas::array_count() const
{
	return (int)roots.size();
}

void TextureAtlas::add_new_root(const Size &texture_size)
{
	auto active_root = std::make_shared<TextureAtlasNode>(Rect(Point(0, 0), texture_size));
	roots.push_back(active_root);
}

/////////////////////////////////////////////////////////////////////////

TextureAtlasNode *TextureAtlasNode::insert(const Size &texture_size, int texture_id)
{
	// If we're not a leaf
	if (child[0] && child[1])
	{
		// Try inserting into first child
		TextureAtlasNode *new_node = child[0]->insert(texture_size, texture_id);
		if (new_node)
			return new_node;

		// No room, insert into second
		return child[1]->insert(texture_size, texture_id);
	}
	else
	{
		// If there's already a texture here, return
		if (id)
			return nullptr;

		// If we're too small, return
		if (texture_size.width > node_rect.get_width() || texture_size.height > node_rect.get_height())
			return nullptr;

		// If we're just right, accept
		if (texture_size.width == node_rect.get_width() && texture_size.height == node_rect.get_height())
		{
			id = texture_id;
			image_rect = node_rect;
			return this;
		}

		// Otherwise, decide which way to split
		int dw = node_rect.get_width() - texture_size.width;
		int dh = node_rect.get_height() - texture_size.height;

		if (dw > dh)
		{
			child[0].reset(new TextureAtlasNode(Rect(node_rect.left, node_rect.top, node_rect.left + texture_size.width, node_rect.bottom)));
			child[1].reset(new TextureAtlasNode(Rect(node_rect.left + texture_size.width, node_rect.top, node_rect.right, node_rect.bottom)));
		}
		else
		{
			child[0].reset(new TextureAtlasNode(Rect(node_rect.left, node_rect.top, node_rect.right, node_rect.top + texture_size.height)));
			child[1].reset(new TextureAtlasNode(Rect(node_rect.left, node_rect.top + texture_size.height, node_rect.right, node_rect.bottom)));
		}

		// Insert into first child we created
		return child[0]->insert(texture_size, texture_id);
	}
}
