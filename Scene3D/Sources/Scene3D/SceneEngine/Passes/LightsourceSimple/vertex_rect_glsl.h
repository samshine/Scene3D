namespace { const char *vertex_rect_glsl() { return R"shaderend(

in vec4 AttrPositionInObject;

flat out vec4 PixelPositionInEye;
flat out vec4 PixelColor;
flat out vec4 PixelRange; // pow(attenuation_end, 2), pow(attenation_start, 2), 1/pow(attenuation_end-attenuation_start, 2), hotspot
flat out vec4 PixelSpotX;
flat out vec4 PixelSpotY;
flat out vec4 PixelSpotZ;
flat out int PixelInstanceId;

layout(std140) uniform Uniforms
{
	mat4 ObjectToEye;
	mat4 EyeToProjection;
	float rcp_f;
	float rcp_f_div_aspect;
	vec2 two_rcp_viewport_size;
};

uniform sampler1D InstanceTexture;

void main()
{
	int LightOffset = gl_InstanceID * 6;

	PixelPositionInEye = texelFetch(InstanceTexture, LightOffset + 0, 0);
	PixelColor = texelFetch(InstanceTexture, LightOffset + 1, 0);
	PixelRange = texelFetch(InstanceTexture, LightOffset + 2, 0);
	PixelSpotX = texelFetch(InstanceTexture, LightOffset + 3, 0);
	PixelSpotY = texelFetch(InstanceTexture, LightOffset + 4, 0);
	PixelSpotZ = texelFetch(InstanceTexture, LightOffset + 5, 0);
	PixelInstanceId = gl_InstanceID;

	gl_Position = AttrPositionInObject;
}

)shaderend"; } }
