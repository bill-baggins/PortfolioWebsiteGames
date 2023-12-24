#version 460 core

layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec3 a_color;
layout (location = 2) in vec2 tex_coords;

out vec4 vert_color;
out vec2 tex_coord;

uniform float time_elapsed;
uniform float dt;
uniform vec2 pos;

void main() 
{
	// vert_color = vec4(a_color, 1.0);
	// oscillates the color of the texture.
	vert_color = vec4(
		0.5 + cos(cos(time_elapsed) + a_color.x),
		0.5 + sin(sin(time_elapsed) + a_color.y),
		0.5 + cos(sin(time_elapsed) + a_color.z),
		1.0
	);
	
	tex_coord = tex_coords;

	vec2 new_pos = a_pos.xy + pos;

	gl_Position = vec4(new_pos, 0.0, 1.0);
}


// spins in a circle.
//	gl_Position = vec4(
//		0.5 * cos(time) + a_pos.x,
//		0.5 * sin(time) + (a_pos.y),
//		a_pos.z,
//		1.0
//	);
