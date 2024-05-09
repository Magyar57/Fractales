#include <iostream>
#include <sstream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "ExternalLibs/imgui.h"
#include "ExternalLibs/imgui_impl_glfw.h"
#include "ExternalLibs/imgui_impl_opengl3.h"

#include "JuliaWindow.hpp"
#include "Utils.hpp"

#define SCROLL_SENSITIVITY 0.1

JuliaWindow* JuliaWindow::gJuliaWindow = nullptr;

JuliaWindow::JuliaWindow(int windowWidth, int windowHeight) : ui(&c_re, &c_im, &min_re, &max_re, &min_im, &max_im) {
	this->windowWidth = windowWidth;
	this->windowHeight = windowHeight;

	// Setup the viewport for the complex plane
	setViewport();
}

JuliaWindow::~JuliaWindow(){
	// Destroy if the app wasn't destroyed already
	if (window != nullptr) destroy();
}

inline static void initRectangle(GLuint& vbo, GLuint& ibo, GLuint& vao){
	// Points des triangles
	GLfloat vertices_pos[] = {
		-1.0f,	1.0f,	0.0f,
		1.0f,	1.0f,	0.0f,
		1.0f,	-1.0f,	0.0f,
		-1.0f,	-1.0f,	0.0f,
	};

	GLuint vertices_indices[] = {
		0, 1, 2, // triangle 0
		0, 2, 3, // triangle 1
	};

	// Buffer de positions
	glGenBuffers(1, &vbo); // Alloue la mémoire dans la cg
	glBindBuffer(GL_ARRAY_BUFFER, vbo); // Sélectionne vbo comme le buffer actuel
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_pos), vertices_pos, GL_STATIC_DRAW); // Copie des données

	// Vertex Array de notre triangle
	// Nécéssaire en core openGL
	// Contient/référence les informations des buffers précédents (pour qu'on ait pas à les utiliser pour dessiner)
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Buffer d'indices
	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(vertices_indices), vertices_indices, GL_STATIC_DRAW);

	// Dire à la CG que les données qu'on lui passe (vertices_pos) sont des coordonnées de vertex (x,y,z)
	// Plus spécfiquement, dire au Vertex Shader comment les vertices_pos sont organisés (layed out)
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL); // description des attributs
	glEnableVertexAttribArray(0);
}

void JuliaWindow::init(){
	window = initOpenGL(windowName, windowWidth, windowHeight);
	if (window == nullptr) exit(-1);
	
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	(void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	// Setup Dear ImGui style ; more: https://www.unknowncheats.me/forum/c-and-c-/189635-imgui-style-settings.html
	ImGui::StyleColorsDark();

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");

	glClearColor(0.12f, 0.12f, 0.12f, 1.0f);
	glfwSetKeyCallback(window, onKey);
	glfwSetScrollCallback(window, onMouseScroll);
	glfwSetFramebufferSizeCallback(window, onFrameBufferResize);
	// glfwSetCursorPosCallback(window, onMouseMove); // buggy with ImGui ??????
	// glfwSwapInterval(0); // Désactiver la VSync

	initRectangle(vbo, ibo, vao);

	juliaShader.loadShaders(SHADERS_PATH + "Julia.vert", SHADERS_PATH + "Julia.frag");

	JuliaWindow::gJuliaWindow = this;
}

void JuliaWindow::run(){
	init();

	while(!glfwWindowShouldClose(window)){
		showFPS(window, windowName);

		// ======
		// Update
		// ======

		glfwPollEvents();
		GLfloat time = glfwGetTime();

		// ====
		// Draw
		// ====

		glClear(GL_COLOR_BUFFER_BIT); // ou DEPTH ou PENCIL

		// Draw Julia fractal
		juliaShader.use();
		juliaShader.setUniform("c_re", c_re);
		juliaShader.setUniform("c_im", c_im);
		juliaShader.setUniform("min_re", min_re);
		juliaShader.setUniform("max_re", max_re);
		juliaShader.setUniform("min_im", min_im);
		juliaShader.setUniform("max_im", max_im);
		glBindVertexArray(vao);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		// Draw UI
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ui.draw();
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
	}
}

void JuliaWindow::destroy(){
	window = nullptr;
	
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ibo);

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwTerminate();
}

void JuliaWindow::setViewport(){
	// Fenêtre carrée
	if (windowWidth == windowHeight){
		min_re = -1.0f;
		max_re = +1.0f;
		min_im = -1.0f;
		max_im = +1.0f;
	}

	// Fenêtre plus haute que large
	if (windowWidth < windowHeight){
		// float aspectRatio = ((float)windowWidth / windowHeight);
		float toRemove = 1.0f - (windowHeight - windowWidth)/(float)windowHeight;

		min_re = -toRemove;
		max_re = +toRemove;
		min_im = -1.0f;
		max_im = +1.0f;
	}

	// Fenêtre plus large que haute
	if (windowWidth > windowHeight) {
		// float aspectRatio = ((float)windowHeight / windowWidth);
		// float toRemove = 1.0f - aspectRatio/2.0f;
		float toRemove = 1.0f - (windowWidth - windowHeight)/(float)windowWidth;
		min_re = -1.0f;
		max_re = +1.0f;
		min_im = -toRemove;
		max_im = +toRemove;
	}
}

void JuliaWindow::onKey(GLFWwindow* fen, int key, int scancode, int action, int mode){
	// W
	if (key==GLFW_KEY_Z && action==GLFW_PRESS){
		gJuliaWindow->wireframe = !gJuliaWindow->wireframe;
		gJuliaWindow->wireframe ? glPolygonMode(GL_FRONT_AND_BACK, GL_LINE) : glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	// D
	if (key==GLFW_KEY_D && action==GLFW_PRESS){
		gJuliaWindow->darkMode = !gJuliaWindow->darkMode;
		(gJuliaWindow->darkMode) ? ImGui::StyleColorsDark() : ImGui::StyleColorsLight();
	}

	// S
	if (key==GLFW_KEY_S && action==GLFW_PRESS){
		gJuliaWindow->ui.toggleVisibility();
	}
}

void JuliaWindow::onFrameBufferResize(GLFWwindow* fen, int buffer_width, int buffer_height){
	float xscale, yscale;
	glfwGetWindowContentScale(fen, &xscale, &yscale);
	
	gJuliaWindow->windowWidth = buffer_width / xscale;
	gJuliaWindow->windowHeight = buffer_height / yscale;
	glViewport(0, 0, buffer_width, buffer_height);

	JuliaWindow::gJuliaWindow->setViewport();
}

void JuliaWindow::onMouseMove(GLFWwindow* fen, double posX, double posY){
	// nothing yet
}

void JuliaWindow::onMouseScroll(GLFWwindow* fen, double deltaX, double deltaY){
	return; // UNIMPLEMENTED ; this is bugged and I'd rather use the settings UI
	
	double posX, posY;
	glfwGetCursorPos(fen, &posX, &posY);
	int windowWidth, windowHeight;
	glfwGetWindowSize(fen, &windowWidth, &windowHeight);
	
	// Transform mouse position to [-1.0, +1.0]
	// posX = (2*posX)/windowWidth - 1.0;
	// posY = ((2*posY)/windowWidth - 1.0) * -1.0;

	printf("deltaX=%.3f deltaY=%.3f - xpos=%.3f ypos=%.3f\n", deltaX, deltaY, posX, posY);

	float min_re = JuliaWindow::gJuliaWindow->min_re;
	float max_re = JuliaWindow::gJuliaWindow->max_re;
	float min_im = JuliaWindow::gJuliaWindow->min_im;
	float max_im = JuliaWindow::gJuliaWindow->max_im;
	float const dx = (max_re - min_re)*SCROLL_SENSITIVITY;
	float const dy = (max_im - min_im)*SCROLL_SENSITIVITY;

	float vecUnitaireX = (max_re - min_re) * windowWidth;
	float vecUnitaireY = (max_im - min_im) * windowHeight;

	// Zoom in
	if (deltaY > 0.5){
		float minXfactor, maxXfactor, minYfactor, maxYfactor;

		min_re = (1.0-SCROLL_SENSITIVITY)*minXfactor*dx * min_re;
		max_re = (1.0-SCROLL_SENSITIVITY)*maxXfactor*dx * max_re;
		min_im = (1.0-SCROLL_SENSITIVITY)*minYfactor*dy * min_im;
		max_im = (1.0-SCROLL_SENSITIVITY)*maxYfactor*dy * max_im;
	}

	// Unzoom
	if (deltaY < 0.5){
		JuliaWindow::gJuliaWindow->min_re = (1.0+SCROLL_SENSITIVITY)*JuliaWindow::gJuliaWindow->min_re;
		JuliaWindow::gJuliaWindow->max_re = (1.0+SCROLL_SENSITIVITY)*JuliaWindow::gJuliaWindow->max_re;
		JuliaWindow::gJuliaWindow->min_im = (1.0+SCROLL_SENSITIVITY)*JuliaWindow::gJuliaWindow->min_im;
		JuliaWindow::gJuliaWindow->max_im = (1.0+SCROLL_SENSITIVITY)*JuliaWindow::gJuliaWindow->max_im;
	}

	JuliaWindow::gJuliaWindow->min_re = min_re;
	JuliaWindow::gJuliaWindow->max_re = max_re;
	JuliaWindow::gJuliaWindow->min_im = min_im;
	JuliaWindow::gJuliaWindow->max_im = max_im;
}
