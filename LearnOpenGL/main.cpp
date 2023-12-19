// As a TODO, try to absract the loading of a shader into a function.
// Also try to simplify the code regarding vertices.
// Maybe have a struct that takes an array of vertices and indices,
// in which it creates a unique VBO, VAO, and EBO tied to that specific
// struct, and give it a draw function that draws the shape to the screen.

// What's safety?
#define _CRT_SECURE_NO_WARNINGS

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <cstdlib>
#include <cstdio>
#include <cstring>

#define _USE_MATH_DEFINES
#include <cmath>

#include "logging.h"
#include "mytypes.h"

constexpr usize ERROR_BUF_SIZE = 512;

const i32 WINDOW_WIDTH = 800;
const i32 WINDOW_HEIGHT = 600;

const f32 TIME_STEP = 0.0003f;

static f32 normal_color(u8 num);
static void framebuffer_resize_callback(GLFWwindow* window, i32 width, i32 height);

// Returns the ID of a shader.
static u32 load_shader(const char* filepath);
static u32 link_shaders(u32 vertex_shader, u32 fragment_shader);

// NOTE: After logging the compile status, the info log will be cleared.
static void log_shader_status(u32 shader_id, GLint success, char* info_log, usize buffer_size);
static void log_program_status(u32 shader_id, GLint success, char* info_log, usize buffer_size);

static void key_callback(GLFWwindow* window, i32 key, i32 scancode, i32 action, i32 mods);



// Using version 4.6 of OpenGL specification.
i32 main(i32 argc, char* argv[])
{
	i32 toggle_wireframes = 0;

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		LOG_FATAL("Could not create window! Exiting...");
	}
	glfwMakeContextCurrent(window);
	glfwSwapInterval(6);

	i32 success = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	if (!success)
	{
		LOG_FATAL("Could not initialize the GLAD library! Exiting...");
	}

	u32 triangle_vertex_shader = load_shader("triangle.vert");
	u32 triangle_fragment_shader = load_shader("triangle.frag");
	u32 triangle_shader = link_shaders(triangle_vertex_shader, triangle_fragment_shader);

	f32 verts[] = {
		-0.25f, 0.5f, 0.0f, // top left 0
		-0.25f, -0.5f, 0.0f, // bottom left 1
		0.0f, -0.5f, 0.0f, // bottom right 2
		0.0f, 0.5f, 0.0f, // top right 3
		0.5f, 0.5f, 0.0f, // top right right 4
		0.5f, 0.25f, 0.0f, // top right bottom right 5
		0.0f, 0.25f, 0.0f, // origin 6
	};

	u32 indices[] = {
		0, 1, 2,
		3, 2, 0,
		/*4, 5, 6,
		3, 4, 6,*/
		3, 4, 2,
	};

	usize indices_length = sizeof(indices) / sizeof(u32);
	usize verts_length = sizeof(verts) / sizeof(f32);

	u32 vbo, vao;
	u32 ebo;

	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);

	glBindVertexArray(vao);

	// Have the vertex buffer hold array buffer data of size verts.
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

	// Uses the indices array to reuse verts in the verts array.
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Steps through the array 3 elements at a time.
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(f32), NULL);
	glEnableVertexAttribArray(0);
	
	// Unbind the buffer for the vbo.
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Unbind the vertex array for this set of vertices.
	glBindVertexArray(0);


	// intitial size set
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

	glfwSetFramebufferSizeCallback(window, framebuffer_resize_callback);
	glfwSetKeyCallback(window, key_callback);
	glfwSetWindowUserPointer(window, &toggle_wireframes);

	u32 u_screen_size = glGetUniformLocation(triangle_shader, "screen_size");
	f32 screen_dimensions[2] = { (f32)WINDOW_WIDTH, (f32)WINDOW_HEIGHT };
	glUniform2fv(u_screen_size, 1, screen_dimensions);
	GLenum mode = GL_FILL;

	while (!glfwWindowShouldClose(window))
	{
		f32 time = glfwGetTime();

		mode = toggle_wireframes ? GL_LINE : GL_FILL;
		glPolygonMode(GL_FRONT_AND_BACK, mode);

		u32 time_loc = glGetUniformLocation(triangle_shader, "time");
		glProgramUniform1fv(triangle_shader, time_loc, 1, &time);

		glClearColor(normal_color(0x87), normal_color(0xce), normal_color(0xeb), 1);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(triangle_shader);
		glBindVertexArray(vao);
		glDrawElements(GL_TRIANGLES, indices_length, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);


		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &vao);
	glDeleteBuffers(1, &ebo);
	glDeleteProgram(triangle_shader);

	glfwDestroyWindow(window);

	glfwTerminate();
	return 0;
}

static void key_callback(GLFWwindow* window, i32 key, i32 scancode, i32 action, i32 mods)
{
	i32* toggle_wireframes = reinterpret_cast<i32*>(glfwGetWindowUserPointer(window));
	switch (key)
	{
		case GLFW_KEY_P:
		{
			*toggle_wireframes = action;
		}
	
	}
}

static void framebuffer_resize_callback(GLFWwindow* window, i32 width, i32 height)
{
	glViewport(0, 0, width, height);
}

// Reads shader code specified by the file path and compiles it.
// According to the stack overflow wizlords, doing an "fseek" to the end on a file
// bigger than 4GB is impossible. However,
static u32 load_shader(const char* filepath)
{
	char info_log[ERROR_BUF_SIZE] = { 0 };

	FILE* file = fopen(filepath, "rb");
	if (file == NULL)
	{
		LOG_WARN("Could not locate the file!");
		return NULL;
	}

	fseek(file, 0, SEEK_END);
	usize length = ftell(file);
	fseek(file, 0, SEEK_SET);
	char* buffer = reinterpret_cast<char*>(malloc(length + 1));

	if (buffer == NULL)
	{
		LOG_FATAL("Could not allocate memory for the buffer!");
	}

	usize elements_read = fread(buffer, sizeof(char), length, file);
	if (elements_read != length)
	{
		LOG_FATAL("READ ERROR: Elements read(%llu) != buffer length(%llu) in %s", elements_read, length, __func__);
	}

	fclose(file);
	buffer[length] = 0;

	GLint shader_success = 0;
	u32 shader_id = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(shader_id, 1, &buffer, NULL);
	glCompileShader(shader_id);
	glGetShaderiv(shader_id, GL_COMPILE_STATUS, &shader_success);
	log_shader_status(shader_id, shader_success, info_log, ERROR_BUF_SIZE);

	free((void*)buffer);

	return shader_id;
}

static u32 link_shaders(u32 vertex_shader, u32 fragment_shader)
{
	char info_log[ERROR_BUF_SIZE] = { 0 };
	GLint success;

	u32 program = glCreateProgram();
	glAttachShader(program, vertex_shader);
	glAttachShader(program, fragment_shader);
	glLinkProgram(program);
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	glGetProgramInfoLog(program, ERROR_BUF_SIZE, 0, info_log);
	log_program_status(program, success, info_log, ERROR_BUF_SIZE);

	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);
}


static void log_shader_status(u32 shader_id, GLint success, char* info_log, usize buffer_size)
{
	if (!success)
	{
		glGetShaderInfoLog(shader_id, buffer_size, NULL, info_log);
		LOG_WARN(info_log);
		memset(info_log, 0, buffer_size);
		return;
	}

	// add parameters for  
	LOG_DEBUG("Compiled shader successfully.");
}

static void log_program_status(u32 program_id, GLint success, char* info_log, usize buffer_size)
{
	if (!success)
	{
		glGetProgramInfoLog(program_id, buffer_size, NULL, info_log);
		LOG_WARN(info_log);
		memset(info_log, 0, buffer_size);
		return;
	}

	// add parameters for  
	LOG_DEBUG("Compiled shader successfully.");
}

static f32 normal_color(u8 num)
{
	return (f32)num / 255;
}
