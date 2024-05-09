#version 330 core

in vec3 vert_pos;

out vec4 frag_color;

vec3 convergence_color = vec3(1.0f, 0.0f, 0.0f);
vec3 divergence_color = vec3(1.0f, 1.0f, 1.0f);

// Julia set
uniform float c_im;
uniform float c_re;
// Viewport
uniform float min_re;
uniform float max_re;
uniform float min_im;
uniform float max_im;

// Retourne la vitesse de divergence de l'ensemble de Julia
float divergenceJuliaSet(float c_re, float c_im, float z0_re, float z0_im){
    float zi_re = z0_re; // init zi = z0
    float zi_im = z0_im;
    float zi_mod = z0_re * z0_re + z0_im * z0_im;

	z0_re *= -1; // Mirror on Ox

    float zi_re_temp;
    const int MAX_ITERATION = 130;
    const float MODULE_MAX = 4.0;

    int nb_iteration = 0;
    while (zi_mod < MODULE_MAX && nb_iteration < MAX_ITERATION) {
        zi_re_temp = zi_re;                             // next zi
        zi_re = zi_re * zi_re - zi_im * zi_im + c_re;   // next zi
        zi_im = 2 * zi_re_temp * zi_im + c_im;          // next zi
        zi_mod = zi_re * zi_re + zi_im * zi_im;   // update zi (squared) module
        nb_iteration++;
    }

	// Si le module est très grand, on a divergé
	if(zi_mod >= MODULE_MAX){
		// Plus on est loin de MAX_ITER (nb_iter proche de 0), plus on a divergé vite
		return 1.0 - (float(nb_iteration) / float(MAX_ITERATION));
	}
	// On a convergé
	else {
		return 0.0;
	}

	// 0.000 - convergence
	// 0.001 - divergence lente
	// 0.999 - divergence rapide
	// 1.000 - divergence instantanée
}

vec3 getFinalColor(float divergence){
	// Divergence
	if (divergence > 0.0){
		// return divergence*divergence_color + (1.0-divergence)*convergence_color; // lerp
		return divergence*divergence_color; // different color for: VERY SLOW DIVERGENCE vs CONVERGENCE
	}

	// Convergence
	return convergence_color;
}

void main(){
	// Compute Julia stuff
	float x_coeff = (vert_pos.x + 1.0f) / 2.0f; // € [0.0 ; 1.0]
	float y_coeff = (vert_pos.y + 1.0f) / 2.0f; // € [0.0 ; 1.0]
	float z0_re = min_re + (max_re-min_re)*x_coeff;
	float z0_im = min_im + (max_im-min_im)*y_coeff;
	float div_speed = divergenceJuliaSet(c_re, c_im, z0_re, z0_im);
	// div_speed = -2.0*div_speed*div_speed*div_speed + 3.0*div_speed*div_speed; // Smooth: y = -2x^3 + 3x²

	// Attribute a color depending on the divergence
	vec3 final_color = getFinalColor(div_speed);
	frag_color = vec4(final_color, 1.0f);
}
