import java.awt.Color;

public class Case {
	Panneau panneau;
	boolean rouge;
	boolean vert;
	boolean bleu;
	int secteurAngulaire;
	int rayon;
	int Incr;
	int X;
	int Y;
	static int tailleFen=700;
	static int gap=(int) (0.35*0.4*tailleFen/(Panneau.getPanneauCourant().getNombreLED()));
	static int redX=(int) (0.7*gap);
	static int redY=(int) (0.5*gap);
	static int greenX=(int) (-0.7*gap);
	static int greenY=(int) (0.5*gap);
	static int blueX=0*gap;
	static int blueY=-1*gap;
		
	public Case(Panneau panneau,int secteurAngulaire, int rayon) {
		this.panneau = panneau;
		this.rouge = false;
		this.vert = false;
		this.bleu = false;
		this.secteurAngulaire = secteurAngulaire;
		this.rayon = rayon;
		this.Incr=1;
		this.X=(int)(tailleFen/2+(((rayon+0.5)*(0.3*tailleFen/(Panneau.getPanneauCourant()).getNombreLED()))+0.1*tailleFen)*Math.cos((secteurAngulaire+0.5)*2*Math.PI/Panneau.getPanneauCourant().getAngle()));
		this.Y=(int)(tailleFen/2+((rayon+0.5)*(0.3*tailleFen/(Panneau.getPanneauCourant().getNombreLED()))+0.1*tailleFen)*Math.sin((secteurAngulaire+0.5)*2*Math.PI/Panneau.getPanneauCourant().getAngle()));
	}
	
	public int getX() {
		return X;
	}

	public void setX(int x) {
		X = x;
	}

	public int getY() {
		return Y;
	}

	public void setY(int y) {
		Y = y;
	}

	public int getIncr() {
		return Incr;
	}

	public void setIncr(int incr) {
		Incr = incr;
	}

	public int getSecteurAngulaire() {
		return secteurAngulaire;
	}

	public void setSecteurAngulaire(int secteurAngulaire) {
		this.secteurAngulaire = secteurAngulaire;
	}

	public int getRayon() {
		return rayon;
	}

	public void setRayon(int rayon) {
		this.rayon = rayon;
	}

	public Panneau getPanneau() {
		return panneau;
	}
	public void setPanneau(Panneau panneau) {
		this.panneau = panneau;
	}
	public boolean isRouge() {
		return rouge;
	}
	public void setRouge(boolean rouge) {
		this.rouge = rouge;
		Fenetre.Principal.repaint();
	}
	public boolean isVert() {
		return vert;
	}
	public void setVert(boolean vert) {
		this.vert = vert;
		Fenetre.Principal.repaint();
	}
	public boolean isBleu() {
		return bleu;
	}
	public void setBleu(boolean bleu) {
		this.bleu = bleu;
			Fenetre.Principal.repaint();
		}
	

	public void caseCliquee() {
		System.out.println("Je colore avec l'increment "+this.getIncr());
		switch (this.getIncr()){
		case 0:
			this.setBleu(false);
			this.setVert(false);
			this.setRouge(false);
			this.setIncr(this.getIncr()+1);
			break;
		case 1:
			this.setBleu(true);
			this.setIncr(this.getIncr()+1);
			break;
		case 2:
			this.setBleu(false);
			this.setVert(true);
			this.setIncr(this.getIncr()+1);
			break;
		case 3:
			this.setVert(false);
			this.setRouge(true);
			this.setIncr(this.getIncr()+1);
			break;
		case 4:
			this.setBleu(true);
			this.setIncr(this.getIncr()+1);
			break;
		case 5:
			this.setVert(true);
			this.setBleu(false);
			this.setIncr(this.getIncr()+1);
			break;
		case 6:
			this.setBleu(true);
			this.setRouge(false);
			this.setIncr(this.getIncr()+1);
			break;
		case 7:
			this.setRouge(true);
			this.setIncr(0);
			break;
		default:

		this.setIncr(0);
		System.out.println("bug");
		}
		}
	
	

}
