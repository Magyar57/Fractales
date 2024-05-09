#ifndef __JULIA_WINDOW_HPP__
#define __JULIA_WINDOW_HPP__

#include <iostream>
#include <string>
#include <GLFW/glfw3.h>
#include "Utils.hpp"
#include "UI.hpp"

class JuliaWindow {
	private:
		const std::string windowName = "Fractales";
		
		GLFWwindow* window; // Main GL window
		GLuint vbo, ibo, vao; // Buffers for the fractal image
		ShaderProgram juliaShader;
		SettingsUI ui;
		float c_re = 0.285f;
		float c_im = 0.013f;
		float min_re, max_re, min_im, max_im; // Complex plane limits
		int windowWidth;
		int windowHeight;
		bool wireframe = false;
		bool darkMode = true;

		// A pointer to a static window, so that it is accessible to the static handler functions
		static JuliaWindow* gJuliaWindow;

	private:
		void init();
		void destroy();
		void setViewport();
		static void onKey(GLFWwindow* fen, int key, int scancode, int action, int mode);
		static void onFrameBufferResize(GLFWwindow* fen, int buffer_width, int buffer_height);
		static void onMouseMove(GLFWwindow* fen, double posX, double posY);
		static void onMouseScroll(GLFWwindow* fen, double deltaX, double deltaY);
	public:
		JuliaWindow(int windowWidth = 900, int windowHeight = 600);
		~JuliaWindow();
		void run();
};

#endif
