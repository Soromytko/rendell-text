#version 450 core

layout(location = 0) in vec2 a_VertexPosition;

uniform mat4 u_Matrix;
uniform vec2 u_FontSize;
uniform int u_CharFrom;

layout(std430, binding = 0) buffer textBuffer { uint text[]; };
layout(std430, binding = 1) buffer glyphTransformBuffer { vec4 glyphTransforms[]; };

out vec2 v_UV;
flat out uint v_TextureIndex;

void main()
{
	const uint characterIndex = gl_InstanceID;
	const uint character = text[characterIndex];
	const vec4 glyphTransform = glyphTransforms[characterIndex];
	const vec2 offset = glyphTransform.xy;
	const vec2 scale = glyphTransform.zw;

	gl_Position = u_Matrix * vec4(a_VertexPosition * scale + offset, 0.0, 1.0);
	v_UV = vec2(a_VertexPosition.x, 1.0 - a_VertexPosition.y) * scale / u_FontSize;
	v_TextureIndex = character - u_CharFrom;
}