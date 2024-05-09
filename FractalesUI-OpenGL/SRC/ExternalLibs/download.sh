#!/bin/bash

set -e

# stb_image.h
curl -O https://raw.githubusercontent.com/nothings/stb/master/stb_image.h

# Dear ImGui
wget -O master.zip https://github.com/ocornut/imgui/archive/refs/heads/master.zip
unzip master.zip
rm master.zip
cp imgui-master/*{.cpp,.h} .
cp imgui-master/backends/imgui_impl_glfw.h .
cp imgui-master/backends/imgui_impl_glfw.cpp .
cp imgui-master/backends/imgui_impl_opengl3.h .
cp imgui-master/backends/imgui_impl_opengl3.cpp .
cp imgui-master/backends/imgui_impl_opengl3_loader.h .
rm -rf imgui-master
