
#pragma once

namespace clan
{
	class TextureAtlasObject;
	class TextureAtlasNode;

	class TextureAtlas
	{
	public:
		TextureAtlas(Size texture_size, bool search_previous_textures = false);

		TextureAtlasObject add(Size size);
		int array_count() const;

	private:
		void add_new_root(const Size &texture_size);

		int next_id = 1;
		Size initial_texture_size;
		std::vector<std::shared_ptr<TextureAtlasNode>> roots;
		bool search_previous_textures;
	};

	class TextureAtlasObject
	{
	public:
		TextureAtlasObject(int array_index, Rect box) : array_index(array_index), box(box) { }

		int array_index = 0;
		Rect box;
	};

	class TextureAtlasNode : public std::enable_shared_from_this<TextureAtlasNode>
	{
	public:
		TextureAtlasNode(const Rect &rect) : node_rect(rect) { }

		std::shared_ptr<TextureAtlasNode> insert(const Size &texture_size, int texture_id);
		std::shared_ptr<TextureAtlasNode> find_image_rect(const Rect &new_rect);

		Rect image_rect;

	private:
		std::shared_ptr<TextureAtlasNode> child[2];
		Rect node_rect;

		int id = 0;
	};
}
