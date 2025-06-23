#version 430 core

in vec2 v_UV;
flat in uint v_TextureIndex;
out vec4 o_Color;

uniform sampler2DArray u_Textures;
uniform vec4 u_TextColor;
uniform vec4 u_BackgroundColor;

void main()
{
	const float sampled = texture(u_Textures, vec3(v_UV, v_TextureIndex)).r;
	const float sampledInverse = 1.0 - sampled;

	const vec3 baseColor = u_TextColor.rgb * sampled + u_BackgroundColor.rgb * sampledInverse;
	const float alpha = u_TextColor.a + u_BackgroundColor.a * sampledInverse;
	o_Color = vec4(baseColor, alpha);
}