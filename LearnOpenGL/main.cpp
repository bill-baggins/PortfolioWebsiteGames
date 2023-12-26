// What's safety?
#define _CRT_SECURE_NO_WARNINGS

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <cstdio>
#include <cstring>

#define _USE_MATH_DEFINES
#include <cmath>

#include <memory>

#include "logging.h"
#include "mytypes.h"

constexpr usize ERROR_BUF_SIZE = 512;

const i32 WINDOW_WIDTH = 800;
const i32 WINDOW_HEIGHT = 600;
const f32 TIME_STEP = 0.0003f;

#define Box(type) std::unique_ptr<type>
#define NewBox(type) std::make_unique<type>

struct TextureGL
{
	u32 id;
	i32 width, height;
	i32 nr_channels;
	GLenum active_texture;
	
	explicit TextureGL()
	{
		glGenTextures(1, &id);
		width = 0;
		height = 0;
		nr_channels = 0;
		active_texture = 0;
		_image_data = nullptr;
	}

	void set_active_texture(GLenum p_active_texture)
	{
		active_texture = p_active_texture;
	}

	void bind()
	{
		// Automatically goes to Active texture 0, which is the default active texture.
		// Written explicitly for clarity.
		glActiveTexture(active_texture);
		glBindTexture(GL_TEXTURE_2D, id);
	}

	void load_image_stb(const char* path)
	{
		_image_data = stbi_load(path, &width, &height, &nr_channels, 0);
		if (_image_data == nullptr)
		{
			LOG_WARN("Unable to load image at %s", path);
		}
	}

	template <typename VoidLambda>
	void set_texture_params(const VoidLambda&& texture_params_lambda)
	{
		texture_params_lambda();
	}

	void generate_from_image_data(GLenum target, GLint level, GLint internalformat, GLint border, GLenum format, GLenum type)
	{
		glTexImage2D(target, level, internalformat, width, height, border, format, type, _image_data);
	}

	template <typename VoidLambda>
	void generated_texture_opts(const VoidLambda&& generate_texture_options_lambda)
	{
		generate_texture_options_lambda();
	}

	void unbind()
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void unload_image_stb()
	{
		stbi_image_free((void*)_image_data);
		_image_data = nullptr;
	}

	~TextureGL()
	{
		glDeleteTextures(1, &id);
		if (_image_data != nullptr)
		{
			stbi_image_free((void*)_image_data);
		}
	}

private:
	const u8* _image_data;
};

// Thin wrapper struct around the GL functions for creating a shape our of
// an array of vertices and an array of indices.
struct ShapeGL
{
	u32 vbo;
	u32 vao; 
	u32 ebo;
	u32 texture;

	f32* verts;
	u32* indices;
	usize verts_len;
	usize indices_len;

	// For now, this only creates a simple shape with GL_STATIC_DRAW.
	// Bound to change later on.
	explicit ShapeGL(const f32 c_verts[], const u32 c_indices[], usize c_verts_len, usize c_indices_len)
	{
		verts_len = c_verts_len;
		indices_len = c_indices_len;

		verts = new f32[verts_len];
		indices = new u32[indices_len];
		
		memcpy(verts, c_verts, verts_len * sizeof(f32));
		memcpy(indices, c_indices, indices_len * sizeof(u32));

		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &vbo);
		glGenBuffers(1, &ebo);
	}

	void gen_texture_bufffer()
	{
		glGenTextures(1, &texture);
	}

	void bind_vao()
	{
		glBindVertexArray(vao);
	}

	void bind_vbo(GLenum usage)
	{
		// Have the vertex buffer hold array buffer data of size verts.
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, verts_len * sizeof(f32), verts, usage);
	}

	void bind_ebo(GLenum usage)
	{
		// Uses the indices array to reuse verts in the verts array.
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_len * sizeof(u32), indices, usage);
	}

	void vertex_attrib_pointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void* pointer)
	{
		// Steps through the array 3 elements at a time.
		glVertexAttribPointer(index, size, type, normalized, stride, pointer);
		glEnableVertexAttribArray(index);
	}

	void unbind_vbo()
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void unbind_vao()
	{
		glBindVertexArray(0);
	}

	void draw()
	{
		glBindVertexArray(vao);
		glDrawElements(GL_TRIANGLES, indices_len, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}

	usize verts_size() const noexcept
	{
		return verts_len * sizeof(f32);
	}

	usize indices_size() const noexcept
	{
		return indices_len * sizeof(f32);
	}

	~ShapeGL()
	{
		glDeleteBuffers(1, &vbo);
		glDeleteBuffers(1, &vao);
		glDeleteBuffers(1, &ebo);
		delete[] verts;
		delete[] indices;
	}
};

// Loads shader code from the specified file and returns its ID.
static u32 load_shader(const char* filepath, GLenum type);

// Links the shaders together. Calling this also disposes the vertex and fragment shaders.
static u32 link_shaders(u32 vertex_shader, u32 fragment_shader);

// NOTE: After logging the compile status, the info log will be cleared.
static void log_shader_status(u32 shader_id, GLint success, char* info_log, usize buffer_size);

// NOTE: After logging the compile status, the info log will be cleared.
static void log_program_status(u32 shader_id, GLint success, char* info_log, usize buffer_size);

// Takes a number 0-255 and turns it into a color in the range [0.0f, 1.0f]
static f32 normal_color(u8 num);

// For changing the glViewport dimensions anytime the window is resized.
static void framebuffer_resize_callback(GLFWwindow* window, i32 width, i32 height);

// Listens for keys.
static void key_callback(GLFWwindow* window, i32 key, i32 scancode, i32 action, i32 mods);

// Note: Using version 4.6 of OpenGL specification.
i32 main(i32 argc, char* argv[])
{
	i32 toggle_wireframes = 0;

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "LearnOpenGL", nullptr, nullptr);
	if (window == nullptr)
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

	// STB image global flags.
	stbi_set_flip_vertically_on_load(true);
	
	TextureGL pepe_texture;
	pepe_texture.set_active_texture(GL_TEXTURE0);
	pepe_texture.bind();
	pepe_texture.set_texture_params([]() {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	});
	pepe_texture.load_image_stb("assets/le_pepe.png");
	pepe_texture.generate_from_image_data(GL_TEXTURE_2D, 0, GL_RGBA, 0, GL_RGBA, GL_UNSIGNED_BYTE);
	pepe_texture.generated_texture_opts([]() {
		glGenerateMipmap(GL_TEXTURE_2D);
	});
	pepe_texture.unload_image_stb();
	pepe_texture.unbind();


	TextureGL wall_texture;
	wall_texture.set_active_texture(GL_TEXTURE1);
	wall_texture.bind();
	wall_texture.set_texture_params([]() {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	});
	wall_texture.load_image_stb("assets/wall.jpg");
	wall_texture.generate_from_image_data(GL_TEXTURE_2D, 0, GL_RGB, 0, GL_RGB, GL_UNSIGNED_BYTE);
	wall_texture.generated_texture_opts([]() {
		glGenerateMipmap(GL_TEXTURE_2D);
	});
	wall_texture.unload_image_stb();
	wall_texture.unbind();

	i32 attributeCount;
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &attributeCount);
	LOG_DEBUG("Maximum number of attributes allowed: %d", attributeCount);

	u32 triangle_vertex_shader = load_shader("triangle.vert", GL_VERTEX_SHADER);
	u32 triangle_fragment_shader = load_shader("triangle.frag", GL_FRAGMENT_SHADER);
	u32 triangle_shader = link_shaders(triangle_vertex_shader, triangle_fragment_shader);

	// Setting the active textures.
	glUseProgram(triangle_shader);

	// Get rid of the leading bits in 0x84C (GL_TEXTURE0) to get the active texture numbers 0-15.
	glProgramUniform1i(triangle_shader, glGetUniformLocation(triangle_shader, "pepe"), pepe_texture.active_texture & (~GL_TEXTURE0));
	glProgramUniform1i(triangle_shader, glGetUniformLocation(triangle_shader, "wall"), wall_texture.active_texture & (~GL_TEXTURE0));

	LOG_DEBUG("walltexture active: %d", wall_texture.active_texture);
	LOG_DEBUG("pepe_texture active: %d", pepe_texture.active_texture);

	f32 verts[] = {
		// bottom left
		-0.5f, -0.5f, 0.0f, 1.0f, 0.1f, 0.1f,   0.0f, 0.0f,
		
		// top left
		-0.5f, 0.5f, 0.0f,  0.1f, 1.0f, 0.1f,   0.0f, 1.0f,

		// bottom right
		0.5f, -0.5f, 0.0f,  0.1f, 0.1f, 1.0f,   1.0f, 0.0f,

		// top right
		0.5f, 0.5f, 0.0f,   0.5f, 0.0f, 0.5f,    1.0f, 1.0f,
	};

	u32 left_triangle_indices[] = {
		0, 1, 2,
		3, 2, 1,
	};

	usize indices_length = sizeof(left_triangle_indices) / sizeof(u32);
	usize verts_length = sizeof(verts) / sizeof(f32);


	ShapeGL rect(verts, left_triangle_indices, verts_length, indices_length);
	rect.bind_vao();
	rect.bind_vbo(GL_STATIC_DRAW);
	rect.bind_ebo(GL_STATIC_DRAW);
	rect.vertex_attrib_pointer(0, 3, GL_FLOAT, GL_FALSE, rect.verts_size() / sizeof(f32), (void*)(0 * sizeof(f32)));
	rect.vertex_attrib_pointer(1, 3, GL_FLOAT, GL_FALSE, rect.verts_size() / sizeof(f32), (void*)(3 * sizeof(f32)));
	rect.vertex_attrib_pointer(2, 2, GL_FLOAT, GL_FALSE, rect.verts_size() / sizeof(f32), (void*)(6 * sizeof(f32)));
	rect.unbind_vbo();
	rect.unbind_vao();

	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	glfwSetFramebufferSizeCallback(window, framebuffer_resize_callback);
	glfwSetKeyCallback(window, key_callback);
	glfwSetWindowUserPointer(window, &toggle_wireframes);

	GLenum mode = GL_FILL;
	glm::vec2 rect_pos(0.0f, 0.0f);
	glm::vec2 rect_vel(0.005f, 0.0045f);

	f32 angle = 0.0f;
	f32 angle_increment = 0.0005f;

	while (!glfwWindowShouldClose(window))
	{
		// Update
		f32 time_elapsed = glfwGetTime();
		f32 dt = 1.f / 60.f;

		rect_pos.x += rect_vel.x * dt;
		if (rect_pos.x > 0.5f)
		{
			rect_pos.x = 0.5f;
			rect_vel.x *= -1;
		}
		else if (rect_pos.x < -0.5f)
		{
			rect_pos.x = -0.5f;
			rect_vel.x *= -1;

		}

		rect_pos.y += rect_vel.y * dt;
		if (rect_pos.y > 0.5f)
		{
			rect_pos.y = 0.5f;
			rect_vel.y *= -1;
		}
		if (rect_pos.y < -0.5f)
		{
			rect_pos.y = -0.5f;
			rect_vel.y *= -1;
		}

		angle += angle_increment;
		if (angle > glm::two_pi<f32>())
		{
			angle = 0.0f;
		}

		glm::mat4 transform(1.0f);
		transform = glm::translate(transform, glm::vec3(rect_pos.x, rect_pos.y, 0.0));
		transform = glm::rotate(transform, angle, glm::vec3(0.0f, 0.0f, 1.0f));

		glProgramUniform1fv(triangle_shader, glGetUniformLocation(triangle_shader, "time_elapsed"), 1, &time_elapsed);
		glProgramUniform1fv(triangle_shader, glGetUniformLocation(triangle_shader, "dt"), 1, &dt);
		glProgramUniform2fv(triangle_shader, glGetUniformLocation(triangle_shader, "pos"), 1, glm::value_ptr(rect_pos));
		glProgramUniformMatrix4fv(triangle_shader, glGetUniformLocation(triangle_shader, "transform"), 1, GL_FALSE, glm::value_ptr(transform));

		// Draw
		mode = toggle_wireframes ? GL_LINE : GL_FILL;
		glPolygonMode(GL_FRONT_AND_BACK, mode);

		glClearColor(normal_color(0x87), normal_color(0xce), normal_color(0xeb), 1);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(triangle_shader);
		pepe_texture.bind();
		wall_texture.bind();

		rect.draw();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
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
static u32 load_shader(const char* filepath, GLenum type)
{
	char info_log[ERROR_BUF_SIZE] = { 0 };

	FILE* file = fopen(filepath, "rb");
	if (file == nullptr)
	{
		LOG_WARN("Shader not found: %s : %s", filepath, __func__);
		return -1;
	}

	fseek(file, 0, SEEK_END);
	usize length = ftell(file);
	fseek(file, 0, SEEK_SET);
	char* buffer = new char[length + 1];

	if (buffer == nullptr)
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
	u32 shader_id = glCreateShader(type);
	glShaderSource(shader_id, 1, &buffer, NULL);
	glCompileShader(shader_id);
	glGetShaderiv(shader_id, GL_COMPILE_STATUS, &shader_success);
	log_shader_status(shader_id, shader_success, info_log, ERROR_BUF_SIZE);

	delete[] buffer;

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

	return program;
}


static void log_shader_status(u32 shader_id, GLint success, char* info_log, usize buffer_size)
{
	if (!success)
	{
		glGetShaderInfoLog(shader_id, buffer_size, nullptr, info_log);
		LOG_WARN(info_log);
		memset(info_log, 0, buffer_size);
		return;
	}

	LOG_DEBUG("Compiled shader successfully.");
}

static void log_program_status(u32 program_id, GLint success, char* info_log, usize buffer_size)
{
	if (!success)
	{
		glGetProgramInfoLog(program_id, buffer_size, nullptr, info_log);
		LOG_WARN(info_log);
		memset(info_log, 0, buffer_size);
		return;
	}

	LOG_DEBUG("Compiled shader successfully.");
}

static f32 normal_color(u8 num)
{
	return (f32)num / 255;
}

