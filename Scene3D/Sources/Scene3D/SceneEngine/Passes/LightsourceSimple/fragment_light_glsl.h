namespace { const char *fragment_light_glsl() { return R"shaderend(

// Direct3D's render targets are top-down, while OpenGL uses bottom-up
//#define TOP_DOWN_RENDER_TARGET

flat in vec4 PixelPositionInEye;
flat in vec4 PixelColor;
flat in vec4 PixelRange; // pow(attenuation_end, 2), pow(attenation_start, 2), 1/pow(attenuation_end-attenuation_start, 2), hotspot
flat in vec4 PixelSpotX;
flat in vec4 PixelSpotY;
flat in vec4 PixelSpotZ;
flat in int PixelInstanceId;

out vec4 FragColor;

layout(std140) uniform Uniforms
{
	mat4 ObjectToEye;
	mat4 EyeToProjection;
	float rcp_f;
	float rcp_f_div_aspect;
	vec2 two_rcp_viewport_size;
};

uniform sampler2DArray ShadowMapsTexture;
uniform sampler2D NormalZTexture;
uniform sampler2D DiffuseColorTexture;
uniform sampler2D SpecularColorTexture;
uniform sampler2D SpecularLevelTexture;
uniform sampler2D SelfIlluminationTexture;

float distance_attenuation(vec3 fragment_to_light);
float circle_falloff_attenuation(vec3 shadow_projection);
float rect_falloff_attenuation(vec3 shadow_projection);
float shadow_attenuation(vec3 fragment_to_light, vec3 shadow_projection);

vec3 project_on_shadow_map(vec3 fragment_to_light);

vec3 unproject_direction(vec2 pos);
vec3 unproject(vec2 pos, float eye_z);

float vsm_reduce_light_bleeding(float p_max, float amount);
float vsm_attenuation(vec2 moments, float t);

float lambertian_diffuse_contribution(vec3 light_direction_in_eye, vec3 normal_in_eye);
float phong_specular_contribution(float lambertian, vec3 light_direction_in_eye, vec3 position_in_eye, vec3 normal_in_eye, float glossiness, float specular_level);
float blinn_specular_contribution(float lambertian, vec3 light_direction_in_eye, vec3 position_in_eye, vec3 normal_in_eye, float glossiness, float specular_level);

void main()
{
	int x = int(gl_FragCoord.x);
	int y = int(gl_FragCoord.y);
	ivec2 pos = ivec2(x,y);
	
	vec4 normal_and_z = texelFetch(NormalZTexture, pos, 0);
	vec2 glossiness_and_specular_level = texelFetch(SpecularLevelTexture, pos, 0).xy;

	vec4 material_diffuse_color = texelFetch(DiffuseColorTexture, pos, 0);
	vec3 material_specular_color = texelFetch(SpecularColorTexture, pos, 0).xyz;
	float material_glossiness = glossiness_and_specular_level.x;
	float material_specular_level = glossiness_and_specular_level.y;
	vec3 material_self_illumination = texelFetch(SelfIlluminationTexture, pos, 0).xyz;
	vec3 normal_in_eye = normal_and_z.xyz;
	float z_in_eye = normal_and_z.w;

	vec3 position_in_eye = unproject(vec2(x, y) + 0.5f, z_in_eye);

	vec3 color = vec3(0,0,0);
#if defined(RECT_PASS)
	if (PixelInstanceId == 0)
		color = material_self_illumination;
#endif

	vec3 fragment_to_light = PixelPositionInEye.xyz - position_in_eye;

	vec3 light_direction_in_eye = normalize(fragment_to_light);
	float diffuse_contribution = lambertian_diffuse_contribution(light_direction_in_eye, normal_in_eye);
	float specular_contribution = blinn_specular_contribution(diffuse_contribution, light_direction_in_eye, position_in_eye, normal_in_eye, material_glossiness, material_specular_level);
	vec3 diffuse_color = material_diffuse_color.xyz * PixelColor.xyz;
	vec3 specular_color = material_specular_color * PixelColor.xyz;
	vec3 lit_color = diffuse_color * diffuse_contribution + specular_color * specular_contribution;

	float light_type = PixelSpotX.w;
#if defined(ONLY_OMNI_LIGHTS)
	if (light_type == 0)
	{
		float attenuation = distance_attenuation(fragment_to_light);
		color += attenuation * lit_color;
	}
#elif defined(ONLY_SPOT_LIGHTS)
	if (light_type != 0 && PixelPositionInEye.w < 0.0f)
	{
		float attenuation = distance_attenuation(fragment_to_light);
		vec3 shadow_projection = project_on_shadow_map(fragment_to_light);
		attenuation *= step(0, shadow_projection.z);
		if (light_type == 1)
			attenuation *= circle_falloff_attenuation(shadow_projection);
		else if (light_type == 2)
			attenuation *= rect_falloff_attenuation(shadow_projection);
		color += attenuation * lit_color;
	}
#elif defined(ONLY_SHADOW_SPOT_LIGHTS)
	if (light_type != 0 && PixelPositionInEye.w >= 0.0f)
	{
		float attenuation = distance_attenuation(fragment_to_light);
		vec3 shadow_projection = project_on_shadow_map(fragment_to_light);
		attenuation *= step(0, shadow_projection.z);
		if (light_type == 1)
			attenuation *= circle_falloff_attenuation(shadow_projection);
		else if (light_type == 2)
			attenuation *= rect_falloff_attenuation(shadow_projection);
		attenuation *= shadow_attenuation(fragment_to_light, shadow_projection);
		color += attenuation * lit_color;
	}
#else
	float attenuation = distance_attenuation(fragment_to_light);
	vec3 shadow_projection = project_on_shadow_map(fragment_to_light);
	if (light_type != 0)
		attenuation *= step(0, shadow_projection.z);
	if (light_type == 1)
		attenuation *= circle_falloff_attenuation(shadow_projection);
	else if (light_type == 2)
		attenuation *= rect_falloff_attenuation(shadow_projection);

	float shadow_att = 1.0f;
	if (PixelPositionInEye.w >= 0.0f)
		shadow_att = shadow_attenuation(fragment_to_light, shadow_projection);

	color += attenuation * (shadow_att * lit_color + PixelColor.a * diffuse_color);
#endif

	FragColor = vec4(color, 0);
}

float distance_attenuation(vec3 fragment_to_light)
{
#if defined(USE_QUADRATIC_ATTENUATION)
	float sqr_distance = dot(fragment_to_light, fragment_to_light);
	float sqr_attenuation_start = PixelRange.y;
	float rcp_sqr_delta_attenuation = PixelRange.z;
	return clamp(1.0f - (sqr_distance - sqr_attenuation_start) * rcp_sqr_delta_attenuation, 0, 1);
#else
	float distance = length(fragment_to_light);
	float attenuation_start = PixelRange.y;
	float rcp_attenuation_delta = PixelRange.z;
	return clamp(1.0f - (distance - attenuation_start) * rcp_attenuation_delta, 0, 1);
#endif
}

float circle_falloff_attenuation(vec3 shadow_projection)
{
	float falloff_begin = PixelRange.w; // hotspot
	const float falloff_end = 1.0f;
	return 1.0f - smoothstep(falloff_begin, falloff_end, dot(shadow_projection.xy, shadow_projection.xy));
}

float rect_falloff_attenuation(vec3 shadow_projection)
{
	float falloff_begin = PixelRange.w; // hotspot
	const float falloff_end = 1.414213562373f;
	return 1.0f - smoothstep(falloff_begin, falloff_end, max(shadow_projection.x * shadow_projection.x, shadow_projection.y * shadow_projection.y) * 2.0f);
}

float shadow_attenuation(vec3 fragment_to_light, vec3 shadow_projection)
{
#if defined(TOP_DOWN_RENDER_TARGET)
	shadow_projection.y = -shadow_projection.y;
#endif
	shadow_projection = shadow_projection * 0.5f + 0.5f;

	float shadow_index = PixelPositionInEye.w;
	vec2 moments = texture(ShadowMapsTexture, vec3(shadow_projection.xy, shadow_index), 0).xy;
	return vsm_attenuation(moments, length(fragment_to_light));
}

vec3 project_on_shadow_map(vec3 fragment_to_light)
{
	// the following calculations assume the vector points from the light towards the fragment position:
	vec3 light_to_fragment = -fragment_to_light;

	// project fragment position on shadow map:
	vec3 position_in_shadow_projection = vec3(
		dot(PixelSpotX.xyz, light_to_fragment),
		dot(PixelSpotY.xyz, light_to_fragment),
		dot(PixelSpotZ.xyz, light_to_fragment));
	return vec3(position_in_shadow_projection.xy / position_in_shadow_projection.z, position_in_shadow_projection.z);
}

float vsm_reduce_light_bleeding(float p_max, float amount)
{
	// Remove the [0, Amount] tail and linearly rescale (Amount, 1].
	return smoothstep(amount, 1.0, p_max);
}

float vsm_attenuation(vec2 moments, float t)
{
	float PixelInVsmEpsilon = 0.1f;

	// Standard shadow map comparison
	float p = (t <= moments.x) ? 1.0 : 0.0;

	// Variance shadow mapping
	float variance = moments.y - (moments.x * moments.x);
	variance = max(variance, PixelInVsmEpsilon); // apply min variance
	float d = t - moments.x; // probabilitic upper bound
	float p_max = variance / (variance + d * d);
	p_max = vsm_reduce_light_bleeding(p_max, 0.4);
	return max(p, p_max);
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

float lambertian_diffuse_contribution(vec3 light_direction_in_eye, vec3 normal_in_eye)
{
	return max(dot(normal_in_eye, light_direction_in_eye), 0.0f);
}

float phong_specular_contribution(float lambertian, vec3 light_direction_in_eye, vec3 position_in_eye, vec3 normal_in_eye, float glossiness, float specular_level)
{
	if (lambertian > 0.0f)
	{
		vec3 view_dir = normalize(-position_in_eye);
		vec3 reflection_dir = normalize(-reflect(light_direction_in_eye, normal_in_eye));
		float spec_angle = max(dot(reflection_dir, normal_in_eye), 0.0f);
		float ph_exp = glossiness;
		return specular_level * pow(spec_angle, ph_exp);
	}
	else
	{
		return 0.0f;
	}
}

float blinn_specular_contribution(float lambertian, vec3 light_direction_in_eye, vec3 position_in_eye, vec3 normal_in_eye, float glossiness, float specular_level)
{
	if (lambertian > 0.0f)
	{
		vec3 view_dir = normalize(-position_in_eye);
		vec3 half_dir = normalize(light_direction_in_eye + view_dir);
		float spec_angle = max(dot(half_dir, normal_in_eye), 0.0f);
		float ph_exp = glossiness * 4.0f;
		return specular_level * pow(spec_angle, ph_exp);
	}
	else
	{
		return 0.0f;
	}
}

)shaderend"; } }
