
#include "precomp.h"
#include "AssetCompiler/MapDescription/map_desc.h"

namespace clan
{
	MapDesc::MapDesc()
	{
	}

	MapDesc MapDesc::load(const std::string &filename)
	{
		MapDesc desc;

		JsonValue json = JsonValue::from_json(File::read_text(filename));
		if ((std::string)json["type"] != "map")
			throw Exception("Not a map description file");
		if ((int)json["version"] != 1)
			throw Exception("Unsupported map description file version");

		desc.fbx_filename = PathHelp::make_absolute(PathHelp::get_fullpath(filename), json["fbx_filename"]);

		return desc;
	}

	void MapDesc::save(const std::string &filename)
	{
		JsonValue json = JsonValue::object();
		json["type"] = "map";
		json["version"] = 1;
		json["fbx_filename"] = PathHelp::make_relative(PathHelp::get_fullpath(filename), fbx_filename);

		File::write_text(filename, json.to_json());
	}
}
 