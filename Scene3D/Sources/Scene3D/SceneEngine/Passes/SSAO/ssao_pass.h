
#pragma once

#include "Scene3D/SceneEngine/Passes/scene_pass.h"
#include "Scene3D/SceneEngine/scene_render.h"
#include "Scene3D/SceneEngine/resource.h"

class SSAOPass : public ScenePass
{
public:
	SSAOPass(SceneRender &inout);
	std::string name() const override { return "ssao"; }
	void run() override;

private:
	struct UniformBuffer
	{
		uicore::Vec2f UVToViewA;
		uicore::Vec2f UVToViewB;
		uicore::Vec2f InvFullResolution;
		float NDotVBias;
		float NegInvR2;
		float RadiusToScreen;
		float AOMultiplier;
		float AOStrength;
		float BlurSharpness;
		float PowExponent;
		float Padding1, Padding2, Padding3;
		uicore::Vec2f Scale;
		uicore::Vec2f Offset;
	};

	void create_ssao_shader();
	void create_blur_shader(bool vertical);
	void create_combine_shader();

	SceneRender &inout;

	uicore::ProgramObjectPtr ssao_shader;
	uicore::ProgramObjectPtr blur_vertical_shader;
	uicore::ProgramObjectPtr blur_horizontal_shader;
	uicore::ProgramObjectPtr combine_shader;
	uicore::UniformVector<UniformBuffer> uniforms;
};
