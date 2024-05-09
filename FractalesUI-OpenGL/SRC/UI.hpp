#ifndef __UI_HPP__
#define __UI_HPP__

#include <iostream>
#include "ExternalLibs/imgui.h"

#define MIN_C_RE_DEFAULT -1.0f
#define MAX_C_RE_DEFAULT +1.0f
#define MIN_C_IM_DEFAULT -1.0f
#define MAX_C_IM_DEFAULT +1.0f

class SettingsUI {
	private:
		bool isVisible = false;
		float* c_re;
		float* c_im;
		float* min_re, *max_re, *min_im, *max_im;
		float min_c_re = MIN_C_RE_DEFAULT;
		float max_c_re = MAX_C_RE_DEFAULT;
		float min_c_im = MIN_C_IM_DEFAULT;
		float max_c_im = MAX_C_IM_DEFAULT;

	public:
		SettingsUI(float* c_re, float* c_im, float* min_re, float* max_re, float* min_im, float* max_im);
		void draw();
		void toggleVisibility();
};

#endif
