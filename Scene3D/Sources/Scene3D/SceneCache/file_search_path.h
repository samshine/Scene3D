
#pragma once

class FileSearchPath
{
public:
	std::string search(const std::string &local_path, const std::string &filename) const
	{
		std::string local = uicore::PathHelp::combine(local_path, filename);
		if (uicore::FileHelp::file_exists(local))
			return local;

		for (size_t i = paths.size(); i > 0; i--)
		{
			std::string f = uicore::PathHelp::combine(paths[i-1], filename);
			if (uicore::FileHelp::file_exists(f))
				return f;
		}

		throw uicore::Exception(uicore::string_format("Unable to locate file %1", filename));
	}
	
	std::vector<std::string> paths;
};
