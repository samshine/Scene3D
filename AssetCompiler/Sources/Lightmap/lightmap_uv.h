
#pragma once

namespace clan
{
	class LightmapUV
	{
	public:
		LightmapUV();

		void generate(const std::shared_ptr<ModelData> &model_data);
	};
}
