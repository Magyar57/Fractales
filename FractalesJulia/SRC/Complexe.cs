namespace Fractales;

public class Complexe {

	public double re { get; private set; }
	public double im { get; private set; }
	private double mod = default;

	public static readonly Complexe zero = new Complexe(0,0);

	public Complexe(double partieRelle, double partieImmaginaire) {
		this.re = partieRelle;
		this.im = partieImmaginaire;
	}

	public Complexe(Complexe z)	{
		this.re = z.re;
		this.im = z.im;
		//this.mod = z.mod;
	}

	public double Modulous(){
		this.mod = Math.Sqrt(im * im + re * re);
		return mod;
	}

	public static double Re(Complexe c) => c.re;
	public static double Im(Complexe c) => c.im;
	public static double Mod(Complexe c) => c.Modulous();

	#region Surcharges d'opérateurs

	public static Complexe operator+ (Complexe a, Complexe b) {
		return new Complexe(a.re + b.re, a.im + b.im);
	}

	public static Complexe operator -(Complexe a, Complexe b) {
		return new Complexe(a.re - b.re, a.im - b.im);
	}

	public static Complexe operator* (Complexe z1, Complexe z2) {
		return new Complexe(z1.re*z2.re - z1.im*z2.im, z1.re*z2.im + z1.im*z2.re );
	}

	public static Complexe Pow(Complexe z0, uint exposant) {
		if (exposant == 0) return new Complexe(1); // z^0 = 1 pour tout z
		
		Complexe z = new Complexe(z0);
		for (int i = 1; i < exposant; i++) z *= z0;
		return z;
	}

	// Cast auto
	public static implicit operator Complexe(int i) => new Complexe(i, 0);
	public static implicit operator Complexe(double d) => new Complexe(d, 0);

	#endregion Surcharges d'opérateurs

	public override string ToString() {
		if (this.im == .0) return re.ToString(); // x

		if (this.re == .0){
			if (this.im == 1.0) return "i"; // i
			return im + "i"; // iy
		}

		return re + " + " + im + "i"; // x + iy
	}

	public static void tests() {
		Complexe z1 = new Complexe(0, 1); // i
		Complexe z2 = new Complexe(3, 4); // 3 + 4i
		Console.WriteLine(z1);
		Console.WriteLine("{0} + {1} = {2}", z1, z1, z1 + z1);
		Console.WriteLine("{0}² = {1}", z1, z1*z1);
		Console.WriteLine("{0} + {1} = {2}", z1, z2, z1 + z2);
		Console.WriteLine("({0}) * ({1}) = {2}", z1, z2, z1 * z2);
		Console.WriteLine();

		Complexe complexe = new Complexe(0, 1);
		for (int i = 0; i < 10; i++) {
			Console.WriteLine(complexe + "^" + i + " = " + Complexe.Pow(complexe, (uint) i));
		}

	}

}