using System;

namespace Fractales;

/// <summary> Définis une classe de polynômes complexes à coefficients complèxes
/// Attention, vu que les delegates et le fonction anonymes c'est n'importe-quoi, pour l'instant cela ne fonctionne qu'avec 3 degrés maximum pour l'instant
/// </summary>
public class Polynome {
	public uint degre { get; private set; }
	public Complexe[] facteurs { get; private set; }
	public Func<Complexe, Complexe> func { get; private set; }

	public Polynome(params Complexe[] values) {
		this.degre = (uint)values.Length-1;
		this.facteurs = new Complexe[values.Length];
		for (int i = 0; i < degre+1; i++) facteurs[i] = values[i]; // degré le plus fort au plus faible

		this.func = (Complexe z) => facteurs[0] * z * z + facteurs[1] * z + facteurs[2]; // les delegates et addition ça pue la merde donc on fait comme ça (c'est moche)
		if (degre > 2) throw new Exception("Polynoms of degree >2 are not supported (yet)");
	}

	[Obsolete("This constructeur is deprecated, please use another one. (This one is bugged as hell and makes no sens, halp)", true)]
	public Polynome(int[] non_c_faux_la_signature_est_differente_regarde_ya_un_array_en_plus, params Complexe[] values) {
		this.degre = (uint) values.Length-1;
		this.facteurs = new Complexe[values.Length];
		for(int i=0; i<degre+1; i++) facteurs[i] = values[i]; // degré le plus fort au plus faible

		this.func = (Complexe z) => 0;
		for(int i=0; i<degre+1; ++i) {
			this.func += ((Complexe z) => (Complexe.Pow(z, (uint) i) * facteurs[i]));
			// C'est buggé, à l'évaluation de func il rentre ici avec i=degre+1 pour une raison obscure (et forcément facteurs[degre+1] ça plante)
		}
	}

	#region Polynômes prédéfinis

	// f(z) = c
	public static Polynome Constant(double c) => new Polynome(c);

	// f(z) = z
	public static Polynome Identite() => new Polynome(1, 0);

	// f(z) = z² - 3/4
	public static Polynome CarreMoins3Quarts() => new Polynome(1, 0, -3 / 4.0);

	#endregion

	#region Trucs inutiles mais fun

	// je garde ces méthodes inutiles parce que c'est fun

	// az² + bz + c
	private static Func<Complexe, Complexe> Polynome3eDegre(double a, double b, double c) {
		return (Complexe z) => a * z * z + b * z + c;
	}

	private static Func<Complexe, Complexe> Polynome3eDegre_versionSyntaxeInfernale(int a, int b, int c) => (Complexe z) => a * Complexe.Pow(z, 2) + b * z + c; // alternative des enfers

	#endregion

}
