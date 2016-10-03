namespace { const char *fragment_lens_distortion_glsl() { return R"shaderend(

#version 330

layout(std140) uniform Uniforms
{
	float Aspect; // image width/height
	float Scale;  // 1/max(f)
	float Padding1, Padding2;
	vec4 k;       // lens distortion coefficient 
	vec4 kcube;   // cubic distortion value
};

in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D InputTexture;

void main()
{
	vec2 position = TexCoord - vec2(0.5);

	vec2 p = vec2(position.x * Aspect, position.y);
	float r2 = dot(p, p);
	vec3 f = vec3(1.0) + r2 * (k.rgb + kcube.rgb * sqrt(r2));

	vec3 x = f * position.x * Scale + 0.5;
	vec3 y = f * position.y * Scale + 0.5;

	vec3 c;
	c.r = texture(InputTexture, vec2(x.r, y.r)).r;
	c.g = texture(InputTexture, vec2(x.g, y.g)).g;
	c.b = texture(InputTexture, vec2(x.b, y.b)).b;

	FragColor = vec4(c, 1.0);
}

)shaderend"; } }
