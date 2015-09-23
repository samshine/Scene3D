
#pragma once

namespace uicore
{
	class LightmapUV
	{
	public:
		void generate(const std::shared_ptr<ModelData> &model_data);

	private:
		const int lightmap_channel_index = 4;
	};
}
