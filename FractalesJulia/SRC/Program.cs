using Fractales;

public class Program {

	static void Main(String[] args) {
		Console.WriteLine("Hello world !");
		//Complexe.tests();
		//ImageFractaleJulia.testsGPU();

		ImageFractaleJulia i = new ImageFractaleJulia(Resolution.QuadHD);

		//Polynome p = Polynome.CarreMoins3Quarts();
		Polynome p = new Polynome(1, 0, new Complexe(0.285, 0.013));

		i.GenerateJulia(p);
		//i.AjouterAxes();
		i.SaveImage();
	}

}