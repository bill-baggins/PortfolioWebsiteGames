#version 460 core

layout (location = 0) in vec3 a_pos;

out vec4 vert_color;

uniform float time;

void main() 
{
	vert_color = vec4(
		0.2 + cos(sin(time) + a_pos.x),
		0.3 + sin(cos(time) + a_pos.y),
		sin(sin(time) + a_pos.z),
		1.0
	);

	gl_Position = vec4(0.5 * cos(time) + a_pos.x, 0.5 * sin(time) + a_pos.y, a_pos.z, 1.0);
}


/*
#version 460 core

layout (location = 0) in vec3 a_pos;

out vec4 vert_color;

uniform float time;

void main() 
{
	vert_color = vec4(
		0.5 + cos(time + abs(a_pos.y)),
		0.5 + cos(abs(time + a_pos.x)),
		0.5 + cos(time + a_pos.z),
		1.0
	);
	gl_Position = vec4(a_pos.x, a_pos.y, a_pos.y, 1.0);
}

*/