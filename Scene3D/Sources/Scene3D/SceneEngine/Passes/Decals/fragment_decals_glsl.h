namespace { const char *fragment_decals_glsl() { return R"shaderend(
#version 330

// Direct3D's render targets are top-down, while OpenGL uses bottom-up
// #define TOP_DOWN_RENDER_TARGET

layout(std140) uniform Uniforms
{
	mat4 EyeToProjection;
	float rcp_f;
	float rcp_f_div_aspect;
	vec2 two_rcp_viewport_size;
};

in float Cutoff;
in float Glossiness;
in float SpecularLevel;
in vec3 BoxCenter;
in vec4 BoxX;
in vec4 BoxY;
in vec4 BoxZ;

out vec4 FragColor;

uniform sampler2D FaceNormalZTexture;

uniform sampler2D DiffuseTexture;

vec3 unproject_direction(vec2 pos);
vec3 unproject(vec2 pos, float eye_z);

void main()
{
	int x = int(gl_FragCoord.x);
	int y = int(gl_FragCoord.y);
	ivec2 pos = ivec2(x,y);

	vec4 normal_and_z = texelFetch(FaceNormalZTexture, pos, 0);
	vec3 normal_in_eye = normalize(normal_and_z.xyz);
	float z_in_eye = normal_and_z.w;
	vec3 position_in_eye = unproject(vec2(x, y) + 0.5f, z_in_eye);

	vec3 v = position_in_eye - BoxCenter;
	float vx = dot(v, BoxX.xyz) * BoxX.w;
	float vy = dot(v, BoxY.xyz) * BoxY.w;
	float vz = dot(v, BoxZ.xyz) * BoxZ.w;
	vec3 position_in_object = vec3(vx, vy, vz);

	vec4 clip = vec4(1 - abs(position_in_object), dot(normal_in_eye, BoxZ.xyz) - Cutoff);
	if (clip.x < 0 || clip.y < 0 || clip.z < 0 || clip.w < 0) discard;

	vec4 textureColor = texture2D(DiffuseTexture, position_in_object.xy * 0.5 + 0.5);

	FragColor = textureColor;
}

vec3 unproject_direction(vec2 pos)
{
//	float field_of_view_y_rad = field_of_view_y_degrees * M_PI / 180.0f;
//	float f = 1.0f / tan(field_of_view_y_rad * 0.5f);
//	float rcp_f = 1.0f / f;
//	float rcp_f_div_aspect = 1.0f / (f / aspect);
	vec2 normalized = vec2(pos * two_rcp_viewport_size);
#if defined(TOP_DOWN_RENDER_TARGET)
	normalized.x = normalized.x - 1.0f;
	normalized.y = 1.0f - normalized.y;
#else
	normalized -= 1.0f;
#endif
	return vec3(normalized.x * rcp_f_div_aspect, normalized.y * rcp_f, 1.0f);
}

vec3 unproject(vec2 pos, float eye_z)
{
	return unproject_direction(pos) * eye_z;
}

)shaderend"; } }
