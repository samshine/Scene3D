
#pragma once

#include "Scene3D/SceneEngine/resource.h"

class GaussianBlur
{
public:
	GaussianBlur();

	void vertical(const uicore::GraphicContextPtr &gc, float blur_amount, int sample_count, uicore::Texture2DPtr input, uicore::FrameBufferPtr output);
	void horizontal(const uicore::GraphicContextPtr &gc, float blur_amount, int sample_count, uicore::Texture2DPtr input, uicore::FrameBufferPtr output);

private:
	GaussianBlur(const GaussianBlur&) = delete;
	GaussianBlur &operator=(const GaussianBlur &) = delete;

	struct BlurSetup
	{
		BlurSetup(float blur_amount, int sample_count) : blur_amount(blur_amount), sample_count(sample_count) { }

		float blur_amount;
		int sample_count;
		uicore::ProgramObjectPtr vertical_blur_program;
		uicore::ProgramObjectPtr horizontal_blur_program;
	};

	void setup(const uicore::GraphicContextPtr &gc, float blur_amount, int sample_count);

	std::string vertex_shader_glsl();
	std::string vertex_shader_hlsl();
	std::string fragment_shader_glsl(float blur_amount, int sample_count, bool vertical);
	std::string fragment_shader_hlsl(float blur_amount, int sample_count, bool vertical);

	float compute_gaussian(float n, float theta);
	void compute_blur_samples(int sample_count, float blur_amount, std::vector<float> &sample_weights, std::vector<int> &sample_offsets);

	uicore::PrimitivesArrayPtr prim_array;
	uicore::VertexArrayVector<uicore::Vec4f> gpu_positions;
	uicore::BlendStatePtr blend_state;

	std::vector<BlurSetup> blur_setups;
	size_t blur_setup_index = 0;
};
