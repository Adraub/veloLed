import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;

public class Moustener implements MouseListener {
	public static Case cas;

	public void mouseClicked(MouseEvent e) {
		try {
			System.out.println("");
			cas=CaseClick(e.getX(),e.getY()-25);
			System.out.println("Apelle case cliquée: rayon "+cas.getRayon()+" ,secteur: "+cas.getSecteurAngulaire());
			cas.caseCliquee();
			
		} catch (Exception e2) {
			System.out.println("EXCEPTION RAISED "+ e2);
			//rien
		}
	   
	       
	      }
	
	  public static Case CaseClick(int X, int Y) throws Exception{
		  
		  int tailleFen=(int)Fenetre.getFenetreCourante().getWidth();
		  double rayon=Math.sqrt(Math.pow(X-tailleFen/2,2)+Math.pow((Y-tailleFen/2),2));
		  double angle=0;
		  if(Y>tailleFen/2){
			  angle =Math.acos(((X-tailleFen/2)/rayon));
		  }
		  else{
			  angle = 2*Math.PI-Math.acos(((X-tailleFen/2)/rayon));
		  }
		  int cercle=(int)((rayon-0.1*tailleFen)/(0.3*tailleFen/(Panneau.getPanneauCourant().getNombreLED())));
		  int secteurAngu=(int) (angle/(2*Math.PI/Panneau.getPanneauCourant().getAngle()));
		  System.out.println("point sur le cercle "+cercle+" et au sercteur " +secteurAngu);
		  if(cercle<0||cercle>Panneau.getPanneauCourant().getNombreLED()-1){
				System.out.println("EXCEPTION RAISED 1");
			  throw new Exception();
		  }
		  if(rayon<(0.1*tailleFen)||rayon>0.4*(tailleFen)){
				System.out.println("EXCEPTION RAISED 2");
			  throw new Exception();
		  }
			  
		  return Panneau.rechercheCase(cercle,secteurAngu);
	  }

	@Override
	public void mouseEntered(MouseEvent e) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void mouseExited(MouseEvent e) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void mousePressed(MouseEvent e) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void mouseReleased(MouseEvent e) {
		// TODO Auto-generated method stub
		
	}


}
