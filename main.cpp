/***********************************************************
* A spherical harmonics computation example and playground *
* no warranty implied | use at your own risk               *
* author: Andreas Mantler (ands) | last change: 13.04.2018 *
*                                                          *
* License:                                                 *
* This software is in the public domain.                   *
* Where that dedication is not recognized,                 *
* you are granted a perpetual, irrevocable license to copy *
* and modify this file however you want.                   *
***********************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>
#include "glad/glad.h"
#include "GLFW/glfw3.h"

extern "C"
{
	#define YO_IMPLEMENTATION
	#define YO_NOIMG
	#include "yocto_obj.h"
}

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "imgui.cpp"
#include "imgui_draw.cpp"
#include "imgui_impl_glfw_gl3.cpp"

#include "m_math.h"
#include "s_shader.h"

typedef struct
{
	struct
	{
		GLuint program;
		GLint u_view;
		GLint u_projection;
		GLint u_cubemap;

		GLuint vao, vbo, ibo;
		int indices;

		GLuint texture;
	} sky;

	struct
	{
		GLuint program;
		GLint u_view;
		GLint u_projection;
		GLint u_coefficients;

		GLuint vao, vbo;
		int vertices;

		m_vec3 coefficients[9];
	} mesh;
} scene_t;

static int initScene(scene_t *scene)
{
	// sky
	int vertexSize = 3 * sizeof(float);
	m_vec3 vertices[] =
	{
		{ 1, -1, 1 },
		{ 1, 1, 1 },
		{ 1, 1, -1 },
		{ -1, 1, -1 },
		{ 1, -1, -1 },
		{ -1, -1, -1 },
		{ -1, -1, 1 },
		{ -1, 1, 1 }
	};

	scene->sky.indices = 14;
	uint16_t indices[] = { 0, 1, 2, 3, 4, 5, 6, 3, 7, 1, 6, 0, 4, 2 };

	glGenVertexArrays(1, &scene->sky.vao);
	glBindVertexArray(scene->sky.vao);

	glGenBuffers(1, &scene->sky.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, scene->sky.vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertexSize, (void *)0);
	glEnableVertexAttribArray(0);

	glGenBuffers(1, &scene->sky.ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, scene->sky.ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint16_t) * scene->sky.indices, indices, GL_STATIC_DRAW);

	const char *skyAttribs[] =
	{
		"a_position"
	};

	const char *skyVP =
		"#version 150 core\n"
		"in vec3 a_position;\n"
		"uniform mat4 u_view;\n"
		"uniform mat4 u_projection;\n"
		"out vec3 v_direction;\n"

		"void main()\n"
		"{\n"
		"    vec4 position = u_projection * (u_view * vec4(a_position, 0.0));\n"
		"    gl_Position = position.xyww;\n"
		"    v_direction = a_position;\n"
		"}\n";

	const char *skyFP =
		"#version 150 core\n"
		"in vec3 v_direction;\n"
		"uniform samplerCube u_cubemap;\n"
		"out vec4 o_color;\n"

		"void main()\n"
		"{\n"
		"    o_color = vec4(texture(u_cubemap, v_direction).rgb, 1.0);\n"
		"}\n";

	scene->sky.program = s_loadProgram(skyVP, skyFP, skyAttribs, 1);
	if (!scene->sky.program)
	{
		fprintf(stderr, "Error loading mesh shader\n");
		return 0;
	}
	scene->sky.u_view = glGetUniformLocation(scene->sky.program, "u_view");
	scene->sky.u_projection = glGetUniformLocation(scene->sky.program, "u_projection");
	scene->sky.u_cubemap = glGetUniformLocation(scene->sky.program, "u_cubemap");

	//#define SKY_DIR "cubemaps/colors/"
	//#define SKY_DIR "cubemaps/tantolunden2/"
	#define SKY_DIR "cubemaps/room/"
	//#define SKY_DIR "cubemaps/powerlines/"
	//#define SKY_DIR "cubemaps/bridge3/"
	//#define SKY_DIR "cubemaps/coittower2/"

	const char *skyTextureFiles[] = {
		SKY_DIR "posx.jpg", SKY_DIR "negx.jpg",
		SKY_DIR "posy.jpg", SKY_DIR "negy.jpg",
		SKY_DIR "posz.jpg", SKY_DIR "negz.jpg"
	};
	const m_vec3 skyDir[] = {
		m_v3(1.0f, 0.0f, 0.0f), m_v3(-1.0f, 0.0f, 0.0f),
		m_v3(0.0f, 1.0f, 0.0f), m_v3(0.0f, -1.0f, 0.0f),
		m_v3(0.0f, 0.0f, 1.0f), m_v3(0.0f, 0.0f, -1.0f)
	};
	const m_vec3 skyX[] = {
		m_v3(0.0f, 0.0f, -1.0f), m_v3(0.0f, 0.0f, 1.0f),
		m_v3(-1.0f, 0.0f, 0.0f), m_v3(1.0f, 0.0f, 0.0f),
		m_v3(1.0f, 0.0f, 0.0f), m_v3(-1.0f, 0.0f, 0.0f)
	};
	const m_vec3 skyY[] = {
		m_v3(0.0f, 1.0f, 0.0f), m_v3(0.0f, 1.0f, 0.0f),
		m_v3(0.0f, 0.0f, -1.0f), m_v3(0.0f, 0.0f, 1.0f),
		m_v3(0.0f, 1.0f, 0.0f), m_v3(0.0f, 1.0f, 0.0f)
	};
	glGenTextures(1, &scene->sky.texture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, scene->sky.texture);

	float weightSum = 0.0f;
	for (int i = 0; i < 6; i++)
	{
		int w, h, c;
		unsigned char *stbidata = stbi_load(skyTextureFiles[i], &w, &h, &c, 3);
		if (!stbidata)
		{
			fprintf(stderr, "Error loading sky texture\n");
			return 0;
		}
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, stbidata);

		// also calculate SH coefficients:
		int step = 16;
		for (int y = 0; y < h; y += step)
		{
			unsigned char *p = stbidata + y * w * 3;
			for (int x = 0; x < w; x += step)
			{
				m_vec3 n = m_add3(
					m_add3(
						m_scale3(skyX[i], 2.0f * (x / (w - 1.0f)) - 1.0f),
						m_scale3(skyY[i], -2.0f * (y / (h - 1.0f)) + 1.0f)),
					skyDir[i]); // texelDirection;
				float l = m_length3(n);
				float weight = 1.0f / (l * l * l); // fast approximation of texelSolidAngle
				m_vec3 c_light = m_scale3(m_v3(p[0], p[1], p[2]), weight / 255.0f);
				n = m_normalize3(n);
				scene->mesh.coefficients[0] = m_add3(scene->mesh.coefficients[0], m_scale3(c_light, 0.282095f));
				scene->mesh.coefficients[1] = m_add3(scene->mesh.coefficients[1], m_scale3(c_light, -0.488603f * n.y * 2.0f / 3.0f));
				scene->mesh.coefficients[2] = m_add3(scene->mesh.coefficients[2], m_scale3(c_light, 0.488603f * n.z * 2.0f / 3.0f));
				scene->mesh.coefficients[3] = m_add3(scene->mesh.coefficients[3], m_scale3(c_light, -0.488603f * n.x * 2.0f / 3.0f));
				scene->mesh.coefficients[4] = m_add3(scene->mesh.coefficients[4], m_scale3(c_light, 1.092548f * n.x * n.y / 4.0f));
				scene->mesh.coefficients[5] = m_add3(scene->mesh.coefficients[5], m_scale3(c_light, -1.092548f * n.y * n.z / 4.0f));
				scene->mesh.coefficients[6] = m_add3(scene->mesh.coefficients[6], m_scale3(c_light, 0.315392f * (3.0f * n.z * n.z - 1.0f) / 4.0f));
				scene->mesh.coefficients[7] = m_add3(scene->mesh.coefficients[7], m_scale3(c_light, -1.092548f * n.x * n.z / 4.0f));
				scene->mesh.coefficients[8] = m_add3(scene->mesh.coefficients[8], m_scale3(c_light, 0.546274f * (n.x * n.x - n.y * n.y) / 4.0f));
				p += 3 * step;
				weightSum += weight;
			}
		}

		free(stbidata);
	}
	for (int s = 0; s < 9; s++)
		scene->mesh.coefficients[s] = m_scale3(scene->mesh.coefficients[s], 4.0f * M_M_PI / weightSum);

	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	// mesh
	//yo_scene *yo = yo_load_obj("sphere.obj", true, false);
	yo_scene *yo = yo_load_obj("dog.obj", true, false);
	if (!yo || !yo->nshapes)
	{
		fprintf(stderr, "Error loading obj file\n");
		return 0;
	}

	scene->mesh.vertices = 0;
	for (int i = 0; i < yo->nshapes; i++)
		scene->mesh.vertices += yo->shapes[i].nelems * 3;

	m_vec3 *positions = (m_vec3*)calloc(scene->mesh.vertices, sizeof(m_vec3));
	m_vec3 *normals   = (m_vec3*)calloc(scene->mesh.vertices, sizeof(m_vec3));
	size_t positionsSize = scene->mesh.vertices * sizeof(m_vec3);
	size_t normalsSize   = scene->mesh.vertices * sizeof(m_vec3);

	int n = 0;
	for (int i = 0; i < yo->nshapes; i++)
	{
		yo_shape *shape = yo->shapes + i;
		for (int j = 0; j < shape->nelems * 3; j++)
		{
			positions[n + j] = *(m_vec3*)&shape->pos[shape->elem[j] * 3];
			normals[n + j] = *(m_vec3*)&shape->norm[shape->elem[j] * 3];
		}
		n += shape->nelems * 3;
	}
	yo_free_scene(yo);

	// upload geometry to opengl
	glGenVertexArrays(1, &scene->mesh.vao);
	glBindVertexArray(scene->mesh.vao);

	glGenBuffers(1, &scene->mesh.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, scene->mesh.vbo);
	glBufferData(GL_ARRAY_BUFFER, positionsSize + normalsSize, NULL, GL_STATIC_DRAW);
	unsigned char *buffer = (unsigned char*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	assert(buffer);
	memcpy(buffer, positions, positionsSize);
	memcpy(buffer + positionsSize, normals, normalsSize);
	glUnmapBuffer(GL_ARRAY_BUFFER);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)positionsSize);

	free(positions);
	free(normals);

	const char *attribs[] =
	{
		"a_position",
		"a_normal"
	};

	const char *vp =
		"#version 150 core\n"
		"in vec3 a_position;\n"
		"in vec3 a_normal;\n"
		"uniform mat4 u_view;\n"
		"uniform mat4 u_projection;\n"
		"out vec3 v_normal;\n"

		"void main()\n"
		"{\n"
		"    gl_Position = u_projection * (u_view * vec4(a_position, 1.0));\n"
		"    v_normal = a_normal;\n"
		"}\n";

	const char *fp =
		"#version 150 core\n"
		"in vec3 v_normal;\n"
		"uniform vec3 u_coefficients[9];\n"
		"out vec4 o_color;\n"

		"void main()\n"
		"{\n"
		"    vec3 n = normalize(v_normal);\n"
		"    vec3 SHLightResult[9];\n"
		"    SHLightResult[0] = 0.282095f * u_coefficients[0];\n"
		"    SHLightResult[1] = -0.488603f * n.y * u_coefficients[1];\n"
		"    SHLightResult[2] = 0.488603f * n.z * u_coefficients[2];\n"
		"    SHLightResult[3] = -0.488603f * n.x * u_coefficients[3];\n"
		"    SHLightResult[4] = 1.092548f * n.x * n.y * u_coefficients[4];\n"
		"    SHLightResult[5] = -1.092548f * n.y * n.z * u_coefficients[5];\n"
		"    SHLightResult[6] = 0.315392f * (3.0f * n.z * n.z - 1.0f) * u_coefficients[6];\n"
		"    SHLightResult[7] = -1.092548f * n.x * n.z * u_coefficients[7];\n"
		"    SHLightResult[8] = 0.546274f * (n.x * n.x - n.y * n.y) * u_coefficients[8];\n"
		"    vec3 result = vec3(0.0);\n"
		"    for (int i = 0; i < 9; ++i)\n"
		"        result += SHLightResult[i];\n"
		"    o_color = vec4(result, 1.0);\n"
		"}\n";

	scene->mesh.program = s_loadProgram(vp, fp, attribs, 2);
	if (!scene->mesh.program)
	{
		fprintf(stderr, "Error loading mesh shader\n");
		return 0;
	}
	scene->mesh.u_view = glGetUniformLocation(scene->mesh.program, "u_view");
	scene->mesh.u_projection = glGetUniformLocation(scene->mesh.program, "u_projection");
	scene->mesh.u_coefficients = glGetUniformLocation(scene->mesh.program, "u_coefficients");

	return 1;
}

static void drawScene(scene_t *scene, float *view, float *projection)
{
	glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	//glDisable(GL_CULL_FACE);

	// mesh
	glUseProgram(scene->mesh.program);
	glUniformMatrix4fv(scene->mesh.u_projection, 1, GL_FALSE, projection);
	glUniformMatrix4fv(scene->mesh.u_view, 1, GL_FALSE, view);
	glUniform3fv(scene->mesh.u_coefficients, 9, &scene->mesh.coefficients[0].x);
	glBindVertexArray(scene->mesh.vao);
	glDrawArrays(GL_TRIANGLES, 0, scene->mesh.vertices);

	// sky
	glDepthMask(GL_FALSE);
	glUseProgram(scene->sky.program);
	glUniformMatrix4fv(scene->sky.u_projection, 1, GL_FALSE, projection);
	glUniformMatrix4fv(scene->sky.u_view, 1, GL_FALSE, view);
	glUniform1i(scene->sky.u_cubemap, 0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, scene->sky.texture);
	glBindVertexArray(scene->sky.vao);
	glDrawElements(GL_TRIANGLE_STRIP, scene->sky.indices, GL_UNSIGNED_SHORT, 0);
	glDepthMask(GL_TRUE);
}

static void destroyScene(scene_t *scene)
{
	// sky
	glDeleteProgram(scene->sky.program);
	glDeleteVertexArrays(1, &scene->sky.vao);
	glDeleteBuffers(1, &scene->sky.vbo);
	glDeleteBuffers(1, &scene->sky.ibo);
	glDeleteTextures(1, &scene->sky.texture);

	// mesh
	glDeleteProgram(scene->mesh.program);
	glDeleteVertexArrays(1, &scene->mesh.vao);
	glDeleteBuffers(1, &scene->mesh.vbo);
}

static void fpsCameraViewMatrix(GLFWwindow *window, float *view, bool ignoreInput)
{
	// initial camera config
	static float position[] = { 0.0f, 0.0f, 3.0f };
	static float rotation[] = { 0.0f, 0.0f };

	// mouse look
	static double lastMouse[] = { 0.0, 0.0 };
	double mouse[2];
	glfwGetCursorPos(window, &mouse[0], &mouse[1]);
	if (!ignoreInput && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{
		rotation[0] += (float)(mouse[1] - lastMouse[1]) * -0.2f;
		rotation[1] += (float)(mouse[0] - lastMouse[0]) * -0.2f;
	}
	lastMouse[0] = mouse[0];
	lastMouse[1] = mouse[1];

	float rotationY[16], rotationX[16], rotationYX[16];
	m_rotation44(rotationX, rotation[0], 1.0f, 0.0f, 0.0f);
	m_rotation44(rotationY, rotation[1], 0.0f, 1.0f, 0.0f);
	m_mul44(rotationYX, rotationY, rotationX);

	// keyboard movement (WSADEQ)
	float speed = (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) ? 0.1f : 0.01f;
	float movement[3] = { 0 };
	if (!ignoreInput)
	{
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) movement[2] -= speed;
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) movement[2] += speed;
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) movement[0] -= speed;
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) movement[0] += speed;
		if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) movement[1] -= speed;
		if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) movement[1] += speed;
	}

	float worldMovement[3];
	m_transform44(worldMovement, rotationYX, movement);
	position[0] += worldMovement[0];
	position[1] += worldMovement[1];
	position[2] += worldMovement[2];

	// construct view matrix
	float inverseRotation[16], inverseTranslation[16];
	m_transpose44(inverseRotation, rotationYX);
	m_translation44(inverseTranslation, -position[0], -position[1], -position[2]);
	m_mul44(view, inverseRotation, inverseTranslation); // = inverse(translation(position) * rotationYX);
}

static void error_callback(int error, const char *description)
{
	fprintf(stderr, "Error: %s\n", description);
}

int main(int argc, char* argv[])
{
	glfwSetErrorCallback(error_callback);
	if (!glfwInit()) return 1;
	glfwWindowHint(GLFW_RED_BITS, 8);
	glfwWindowHint(GLFW_GREEN_BITS, 8);
	glfwWindowHint(GLFW_BLUE_BITS, 8);
	glfwWindowHint(GLFW_ALPHA_BITS, 8);
	glfwWindowHint(GLFW_DEPTH_BITS, 32);
	glfwWindowHint(GLFW_STENCIL_BITS, GLFW_DONT_CARE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
	glfwWindowHint(GLFW_SAMPLES, 4);
	GLFWwindow *window = glfwCreateWindow(1280, 800, "Spherical Harmonics Playground", NULL, NULL);
	if (!window) return 1;
	glfwMakeContextCurrent(window);
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	glfwSwapInterval(1);

	ImGui_ImplGlfwGL3_Init(window, true);

	scene_t scene = {0};
	if (!initScene(&scene))
	{
		fprintf(stderr, "Could not initialize scene.\n");
		return 1;
	}

	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
		if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
		{
			destroyScene(&scene);
			if (!initScene(&scene))
			{
				fprintf(stderr, "Could not reinitialize scene.\n");
				break;
			}
		}
		ImGui_ImplGlfwGL3_NewFrame();

		ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiSetCond_FirstUseEver);
		static bool show_another_window = true;
		ImGui::Begin("Coefficients", &show_another_window);
		for (int i = 0; i < 9; i++)
		{
			char name[] = "[?]";
			name[1] = i + '0';
			m_vec3 remapped = m_scale3(m_add3(scene.mesh.coefficients[i], m_v3(1.0f, 1.0f, 1.0f)), 0.5f);
			ImGui::ColorEdit3(name, &remapped.x);
			scene.mesh.coefficients[i] = m_sub3(m_scale3(remapped, 2.0f), m_v3(1.0f, 1.0f, 1.0f));
		}
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();

		int w, h;
		glfwGetFramebufferSize(window, &w, &h);
		glViewport(0, 0, w, h);
		float view[16], projection[16];
		fpsCameraViewMatrix(window, view, ImGui::IsAnyItemActive());
		m_perspective44(projection, 45.0f, (float)w / (float)h, 0.01f, 100.0f);
		drawScene(&scene, view, projection);

		ImGui::Render();
		glfwSwapBuffers(window);
	}

	destroyScene(&scene);
	ImGui_ImplGlfwGL3_Shutdown();
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}