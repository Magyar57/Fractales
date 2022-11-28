using System;
using System.Drawing;

namespace Fractales;

public partial class ImageFractaleJulia {

	private void GenerateJuliaCPU(Func<Complexe, Complexe> suite, double scale) {
		int nbPixels = width * height;
		int nbDone = 0;

		for (int i = 0; i < width; i++) { // pour chaque pixel
			for (int j = 0; j < height; j++) {
				image.SetPixel(i, j, GetColor(suite, new Complexe((i - offset.re) * scale, (j - offset.im) * scale))); // on lui donne une couleur
				nbDone++;
				if (nbDone % 10000 == 0) Console.Write("Generating... {0}% ({1}/{2})\r", 100 * ((float)nbDone) / nbPixels, nbDone, nbPixels); // affichage du % de complétion
			}
		}
		Console.WriteLine("Generating... 100% ({0}/{1}) - Done !", nbDone, nbPixels); // 100% !
	}

	private static Color GetColor(Func<Complexe, Complexe> suite, Complexe z0) {
		return DivergenceNoirEtBlanc(suite, z0);
	}

	private static Color DivergenceNoirEtBlanc(Func<Complexe, Complexe> suite, Complexe z0) {
		return IsDivergente(suite, z0) ? Color.Black : Color.White;
	}

	private static bool IsDivergente(Func<Complexe, Complexe> suite, Complexe z0) {
		Complexe ztemp = new Complexe(z0);
		const uint MAX_ITERATION = 200;
		const double MODULE_MAX = 4.0;

		int i = 0;
		while (ztemp.Modulous() < MODULE_MAX && i < MAX_ITERATION) {
			ztemp = suite(ztemp);
			i++;
		}

		if (ztemp.Modulous() >= MODULE_MAX) { // On a un module très grand
			return true;
		}

		return false;
	}

}
