
#pragma once

class FileSearchPath
{
public:
	std::string search(const std::string &local_path, const std::string &filename) const
	{
		std::string local = clan::PathHelp::combine(local_path, filename);
		if (clan::FileHelp::file_exists(local))
			return local;

		for (size_t i = paths.size(); i > 0; i--)
		{
			std::string f = clan::PathHelp::combine(paths[i-1], filename);
			if (clan::FileHelp::file_exists(f))
				return f;
		}

		throw clan::Exception(clan::string_format("Unable to locate file %1", filename));
	}
	
	std::vector<std::string> paths;
};
