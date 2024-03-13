#version 450 core

layout (location = 0) in vec2 inPos;
//layout (location = 2) in vec2 drawId;
//layout (location = 3) in mat4 inModel;

out VS_OUT {
	vec2 localPos;
	vec2 fragPos;
	vec3 fragColor;
} vs_out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 color;

void main() {
	vec4 pos = projection * view * model * vec4(inPos, 0.0, 1.0);
	
	vs_out.localPos = inPos;
	vs_out.fragPos = vec2(model * vec4(inPos, 0.0, 1.0));
	vs_out.fragColor = color;

	gl_Position = pos;
}