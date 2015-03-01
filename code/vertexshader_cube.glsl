#version 400

layout(location=0) in vec4 in_position;
layout(location=1) in vec4 in_color;
out vec4 ex_color;

uniform mat4 mvp;

void main(void) {
	gl_Position = mvp * in_position;
	ex_color = in_color;
}