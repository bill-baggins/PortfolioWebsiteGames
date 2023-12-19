#version 460 core

in vec4 vert_color;

out vec4 frag_color;

uniform vec2 screen_size;
 
void main()
{
	frag_color = vert_color;
}