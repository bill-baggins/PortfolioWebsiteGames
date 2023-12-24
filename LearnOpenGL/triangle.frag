#version 460 core

in vec4 vert_color;

in vec2 tex_coord;

uniform sampler2D pepe;
uniform sampler2D wall;
 
void main()
{
	gl_FragColor = \
		mix(texture(wall, tex_coord), texture(pepe, vec2(-tex_coord.x, tex_coord.y)), 0.5)
		* vert_color;
}