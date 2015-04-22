
#pragma once

class GameDisplayCache : public clan::DisplayCache
{
public:
	clan::Resource<clan::Sprite> get_sprite(clan::Canvas &canvas, const std::string &id) override;
	clan::Resource<clan::Image> get_image(clan::Canvas &canvas, const std::string &id) override;
	clan::Resource<clan::Texture> get_texture(clan::GraphicContext &gc, const std::string &id) override;
	clan::Resource<clan::Font> get_font(clan::Canvas &canvas, const std::string &family_name, const clan::FontDescription &desc) override;

private:
	std::map<std::string, clan::Resource<clan::Image>> images;
	std::map<std::string, clan::FontFamily> font_families;
};
