#include <iostream>
#include <sstream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "ExternalLibs/imgui.h"
#include "ExternalLibs/imgui_impl_glfw.h"
#include "ExternalLibs/imgui_impl_opengl3.h"

#include "JuliaWindow.hpp"

int main(){
	JuliaWindow jw(1200, 1200);
	jw.run();

	return 0;
}
