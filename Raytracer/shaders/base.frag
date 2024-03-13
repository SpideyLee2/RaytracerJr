#version 450 core

out vec4 FragColor;

in VS_OUT {
	vec2 localPos;
	vec2 fragPos;
	vec3 fragColor;
} fs_in;

void main() {
	FragColor = vec4(fs_in.fragColor, 1.0);
}