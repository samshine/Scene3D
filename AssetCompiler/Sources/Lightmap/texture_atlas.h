
#pragma once

class TextureAtlasObject;
class TextureAtlasNode;

class TextureAtlas
{
public:
	TextureAtlas(uicore::Size texture_size, bool search_previous_textures = false);

	TextureAtlasObject add(uicore::Size size);
	int array_count() const;

private:
	void add_new_root(const uicore::Size &texture_size);

	int next_id = 1;
	uicore::Size initial_texture_size;
	std::vector<std::shared_ptr<TextureAtlasNode>> roots;
	bool search_previous_textures;
};

class TextureAtlasObject
{
public:
	TextureAtlasObject(int array_index, uicore::Rect box) : array_index(array_index), box(box) { }

	int array_index = 0;
	uicore::Rect box;
};

class TextureAtlasNode : public std::enable_shared_from_this<TextureAtlasNode>
{
public:
	TextureAtlasNode(const uicore::Rect &rect) : node_rect(rect) { }

	TextureAtlasNode *insert(const uicore::Size &texture_size, int texture_id);

	uicore::Rect image_rect;

private:
	std::unique_ptr<TextureAtlasNode> child[2];
	uicore::Rect node_rect;

	int id = 0;
};
