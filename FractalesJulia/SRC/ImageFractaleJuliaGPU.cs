using System;
using System.Diagnostics;
using System.Drawing;
using System.Runtime.InteropServices;

namespace Fractales;

public partial class ImageFractaleJulia {

	[DllImport(SHADER_DLL_NAME)] static extern unsafe int** allocate_tab(int n, int m);
	[DllImport(SHADER_DLL_NAME)] static extern unsafe void free_tab(int** tab, int n, int m);
	[DllImport(SHADER_DLL_NAME)] static extern unsafe void calculate_Julia(double c_re, double c_im, int** res, int width, int height, double scale, double offset_re, double offset_im);
	private void GenerateJuliaGPU(Polynome suite, double scale) {

		if (FORCE_CPU) throw new DllNotFoundException(); // Force l'utilisation CPU (voir catch/try dans ImageFractaleJulia.GenerateJulia(...) )

		Stopwatch timer = new Stopwatch();

		unsafe {
			// Allocation préalable du tableau (permet de transmettre les données du dll C++ au programme C#)
			int** res = allocate_tab(this.width, this.height);

			// On appelle la fonction C++ parallélisée
			Console.Write("Generating using GPU... 0%\r");
			timer.Start();
			calculate_Julia(suite.facteurs[2].re, suite.facteurs[2].im, res, width, height, scale, offset.re, offset.im);
			timer.Stop();
			Console.WriteLine("Generating using GPU... 100% - Done in {0} seconds", timer.Elapsed.TotalSeconds);

			// On traite les résultats
			Console.Write("Copying data... 0%\r");
			timer.Restart();
			for (int i=0; i<width; i++) for(int j=0; j<height; j++) image.SetPixel(i, j, GetColorGPU(res[i][j])); // Pour chaque pixel, on lui donne une couleur fonction du résultat du calcul du shader
			timer.Stop();
			Console.WriteLine("Copying data... 100% - Done in {0} seconds", timer.Elapsed.TotalSeconds);
			
			// On libère la mémoire
			free_tab(res, this.width, this.height);
		}

	}

	private static Color GetColorGPU(int is_divergente) {
		return (is_divergente == 0) ? Color.White : Color.Black;
	}

	[DllImport(SHADER_DLL_NAME)] static extern void tests();
	public static void testsGPU() {
		tests();
	}

}
