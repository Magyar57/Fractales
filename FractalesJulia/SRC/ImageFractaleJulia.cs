using System.Drawing;

namespace Fractales;

public struct Resolution {
	public static readonly Tuple<int, int> FullHD = Tuple.Create<int, int>(1920, 1080);
	public static readonly Tuple<int, int> QuadHD = Tuple.Create<int, int>(3840, 2160);
	public static readonly Tuple<int, int> _4K = Tuple.Create<int, int>(7680, 4320);
	public static readonly Tuple<int, int> _8K = Tuple.Create<int, int>(15360, 8640);
};

public partial class ImageFractaleJulia {
	public int width { get; private set; }
	public int height { get; private set; }
	public Complexe offset;
	private Bitmap image;

	private const bool FORCE_CPU = false;
	private const string SHADER_DLL_NAME = "CUDAShader.dll";

	public ImageFractaleJulia(int width, int height, bool centered = true) {
		this.width = width;
		this.height = height;
		this.offset = centered ? new Complexe(width / 2, height / 2) : new Complexe(0, 0);
		image = new Bitmap(width, height);
	}

	public ImageFractaleJulia(Tuple<int,int> resolution, bool centered = true) {
		this.width = resolution.Item1;
		this.height = resolution.Item2;
		this.offset = centered ? new Complexe(width / 2, height / 2) : new Complexe(0, 0);
		image = new Bitmap(resolution.Item1, resolution.Item2);
	}

	public void GenerateJulia(Polynome suite, double scale = -1) {
		if (scale < 0) scale = 4.0 / this.width;
		
		try {
			GenerateJuliaGPU(suite, scale);
		}
		catch(DllNotFoundException) {
			Console.WriteLine("Le dll \"{0}\" n'a pas été trouvé. Exécution sur CPU", SHADER_DLL_NAME);
			GenerateJuliaCPU(suite.func, scale);
		}
	}

	public void AjouterAxes() {
		int middle_w = (int) offset.im;
		int middle_h = (int) offset.re;
		// si l'axe est bien sur l'écran en largeur => pour tous les pixels en hauteur => pour la largeur fixée, on colore en rouge
		if(middle_w>=0 && middle_w<width) for (int i = 0; i < width; i++) image.SetPixel(i, middle_w, Color.Red);
		if (middle_h >= 0 && middle_h < height) for (int i = 0; i < height; i++) image.SetPixel(middle_h, i, Color.Red); // même principe
	}

	public void SaveImage() {
		image.Save(@"./ma_super_fractale.png");
	}

}

/*
t'as une classe Bitmap dans le namespace System.Drawing;
Bitmap b = new Bitmap(1920, 1080);
Color c = b.GetPixel(10, 20);
c = Color.FromArgb(255, 255, 255, 255)
b.SetPixel(10, 10, c)
b.Save(@"/c:/user/ect");
*/