#include <iostream>

#include "UI.hpp"

static const float INTERESTING_VALUES[][2] = {
	{-1.0000f, +0.0000f}, // Mandelbrot
	{+0.3000f, +0.5000f}, // 0.3 + 0.5i
	{-1.4760f, +0.0000f}, // -1.476
	{+0.2850f, +0.0100f}, // 0.285 + 0.01i
	{-1.4170f, +0.0099f}, // –1.4170 + 0.0099i
	{-0.4000f, +0.6000f}, // −0.4 + 0.6i
};

SettingsUI::SettingsUI(float* c_re, float* c_im, float* min_re, float* max_re, float* min_im, float* max_im){
	this->c_re = c_re;
	this->c_im = c_im;
	this->min_re = min_re;
	this->max_re = max_re;
	this->min_im = min_im;
	this->max_im = max_im;
}

void SettingsUI::draw(){
	if (!isVisible) return;

	ImGuiIO& io = ImGui::GetIO();
	ImGui::Begin("Fractales settings");

	ImGui::SeparatorText("Julia set parameters");
	
	ImGui::PushItemWidth(64.0f);
	ImGui::InputFloat("##c_re", c_re);
	ImGui::PopItemWidth();
	ImGui::SameLine();
	ImGui::SliderFloat("Re(C)", c_re, min_c_re, max_c_re, "%.4f");

	ImGui::PushItemWidth(64.0f);
	ImGui::InputFloat("##c_im", c_im);
	ImGui::PopItemWidth();
	ImGui::SameLine();
	ImGui::SliderFloat("Im(C)", c_im, min_c_im, max_c_im, "%.4f");

	if (ImGui::CollapsingHeader("Sliders settings")){
		ImGui::InputFloat("Slider min - Re(C)", &min_c_re);
		ImGui::InputFloat("Slider max - Re(C)", &max_c_re);
		ImGui::InputFloat("Slider min - Im(C)", &min_c_im);
		ImGui::InputFloat("Slider max - Im(C)", &max_c_im);
	}

	if (ImGui::Button("Reset")){
		*c_re = 0.285f;
		*c_im = 0.013f;
		min_c_re = MIN_C_RE_DEFAULT;
		max_c_re = MAX_C_RE_DEFAULT;
		min_c_im = MIN_C_IM_DEFAULT;
		max_c_im = MAX_C_IM_DEFAULT;
	}

	ImGui::SeparatorText("Viewport (complex plane) parameters");
	ImGui::InputFloat("Min Re", min_re);
	ImGui::InputFloat("Max Re", max_re);
	ImGui::InputFloat("Min Im", min_im);
	ImGui::InputFloat("Max Im", max_im);

	ImGui::SeparatorText("Some interesting values");
	if (ImGui::CollapsingHeader("Display")){
		for(int i=0 ; i<sizeof(INTERESTING_VALUES)/(sizeof(float)*2) ; i++){
			ImGui::Bullet();
			ImGui::SameLine();

			char buff[10]; // sizeof("Set 99") = 7UL so we don't really need 10
			sprintf(buff, "Set %.3d", i+1);
			if (ImGui::Button(buff)){ *c_re = INTERESTING_VALUES[i][0]; *c_im = INTERESTING_VALUES[i][1]; }
			
			ImGui::SameLine();
			ImGui::Text("C = %.4f + %.4fi", INTERESTING_VALUES[i][0], INTERESTING_VALUES[i][1]);
		}
	}

	ImGui::SeparatorText("About");
	ImGui::Text("'Fractales' project, by Alexandre Brochart, 2024");
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);

	ImGui::End();
}

void SettingsUI::toggleVisibility(){
	this->isVisible = !this->isVisible;
}
