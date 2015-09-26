
#pragma once

#include "Scene3D/SceneCache/resource.h"

class GaussianBlur
{
public:
	GaussianBlur();
	void blur(uicore::GraphicContext &gc, uicore::TextureFormat format, float blur_amount, int sample_count = 15);

	Resource<uicore::Texture2D> input;
	Resource<uicore::FrameBuffer> output;

private:
	void setup(uicore::GraphicContext &gc, uicore::Size new_size, uicore::TextureFormat new_format, float blur_amount, int sample_count);

	void get_shader_glsl(float blur_amount, int sample_count, std::string &out_vertex_shader, std::string &out_vertical_fragment_shader, std::string &out_horizontal_fragment_shader);
	void get_shader_hlsl(float blur_amount, int sample_count, std::string &out_vertex_shader, std::string &out_vertical_fragment_shader, std::string &out_horizontal_fragment_shader);
	float compute_gaussian(float n, float theta);
	void compute_blur_samples(int sample_count, float blur_amount, int dx, int dy, std::vector<float> &sample_weights, std::vector<uicore::Vec2i> &sample_offsets);

	struct BlurSetup
	{
		BlurSetup(float blur_amount, int sample_count) : blur_amount(blur_amount), sample_count(sample_count) { }

		float blur_amount;
		int sample_count;
		uicore::ProgramObject vertical_blur_program;
		uicore::ProgramObject horizontal_blur_program;
	};

	uicore::PrimitivesArray prim_array;
	uicore::VertexArrayVector<uicore::Vec4f> gpu_positions;
	uicore::FrameBuffer fb0;
	uicore::Texture2D pass0_texture;
	uicore::Size size;
	uicore::TextureFormat format;
	std::vector<BlurSetup> blur_setups;
	std::vector<BlurSetup>::iterator current_blur_setup;
	uicore::BlendState blend_state;
};
