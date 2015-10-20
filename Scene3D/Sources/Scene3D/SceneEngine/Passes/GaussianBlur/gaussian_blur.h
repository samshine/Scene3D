
#pragma once

#include "Scene3D/SceneEngine/resource.h"

class GaussianBlur
{
public:
	GaussianBlur();
	void blur(const uicore::GraphicContextPtr &gc, uicore::TextureFormat format, float blur_amount, int sample_count = 15);

	uicore::Texture2DPtr input;
	uicore::FrameBufferPtr output;

private:
	void setup(const uicore::GraphicContextPtr &gc, uicore::Size new_size, uicore::TextureFormat new_format, float blur_amount, int sample_count);

	void get_shader_glsl(float blur_amount, int sample_count, std::string &out_vertex_shader, std::string &out_vertical_fragment_shader, std::string &out_horizontal_fragment_shader);
	void get_shader_hlsl(float blur_amount, int sample_count, std::string &out_vertex_shader, std::string &out_vertical_fragment_shader, std::string &out_horizontal_fragment_shader);
	float compute_gaussian(float n, float theta);
	void compute_blur_samples(int sample_count, float blur_amount, int dx, int dy, std::vector<float> &sample_weights, std::vector<uicore::Vec2i> &sample_offsets);

	struct BlurSetup
	{
		BlurSetup(float blur_amount, int sample_count) : blur_amount(blur_amount), sample_count(sample_count) { }

		float blur_amount;
		int sample_count;
		uicore::ProgramObjectPtr vertical_blur_program;
		uicore::ProgramObjectPtr horizontal_blur_program;
	};

	uicore::PrimitivesArrayPtr prim_array;
	uicore::VertexArrayVector<uicore::Vec4f> gpu_positions;
	uicore::FrameBufferPtr fb0;
	uicore::Texture2DPtr pass0_texture;
	uicore::Size size;
	uicore::TextureFormat format;
	std::vector<BlurSetup> blur_setups;
	std::vector<BlurSetup>::iterator current_blur_setup;
	uicore::BlendStatePtr blend_state;
};