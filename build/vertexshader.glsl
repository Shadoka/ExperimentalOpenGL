#version 400

layout(location=0) in vec4 in_Position;
layout(location=1) in vec4 in_Color;
out vec4 ex_color;

void main(void) {
 gl_Position = in_Position;
 ex_color = in_Color;
}