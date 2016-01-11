import java.awt.Color;
import java.awt.Graphics;
import java.awt.Graphics2D;

import javax.swing.JPanel;

public class Panel extends JPanel { 
	public static Graphics g;



	public void paintComponent(Graphics g){
		  for(int i = 0; i < Panneau.PanneauCourant.listeCase.length; i++)
			{
				Case cas=Panneau.PanneauCourant.listeCase[i];
				
				if(( cas).isBleu()==false){
					g.setColor(Color.BLACK);
					g.fillOval((int)(cas.getX()-Case.blueX/2),(int)(cas.getY()-Case.blueY/2),Case.gap,Case.gap);
				}
				
				if(( cas).isVert()==false){
					g.setColor(Color.BLACK);
					g.fillOval((int)(cas.getX()-Case.greenX/2),(int)(cas.getY()-Case.greenY/2),Case.gap,Case.gap);
				}
				if(( cas).isRouge()==false){
					g.setColor(Color.BLACK);
					g.fillOval((int)(cas.getX()-Case.redX/2),(int)(cas.getY()-Case.redY/2),Case.gap,Case.gap);
				}
				if(( cas).isRouge()){
					g.setColor(Color.RED);
					g.fillOval((int)(cas.getX()-Case.redX/2),(int)(cas.getY()-Case.redY/2),Case.gap,Case.gap);
				}
				if(( cas).isVert()){
					g.setColor(Color.GREEN);
					g.fillOval((int)(cas.getX()-Case.greenX/2),(int)(cas.getY()-Case.greenY/2),Case.gap,Case.gap);
				}
				if(( cas).isBleu()){
					g.setColor(Color.BLUE);
					g.fillOval((int)(cas.getX()-Case.blueX/2),(int)(cas.getY()-Case.blueY/2),Case.gap,Case.gap);
				}
				
			}
		  g.setColor(Color.WHITE) ;
		  int tailleFen=(int)Fenetre.getFenetreCourante().getWidth();
		  for(int i = 0; i < Panneau.getPanneauCourant().getNombreLED()+1; i++)
		  {
			  int position=(int) ((int)tailleFen*(0.1+i*0.3/(Panneau.getPanneauCourant().getNombreLED())));
			  g.drawOval(position, position,tailleFen-2*position , tailleFen-2*position);

		  }
		  int rayon=(int) (0.4*tailleFen);
		  for(int i = 0; i < Panneau.getPanneauCourant().getAngle(); i++)
		  {
			  g.drawLine(tailleFen/2, tailleFen/2, (int)(tailleFen/2+rayon*Math.cos(i*2*Math.PI/Panneau.getPanneauCourant().getAngle())), (int)(tailleFen/2+rayon*Math.sin(i*2*Math.PI/Panneau.PanneauCourant.getAngle())));
		  }
		  g.fillOval((int)(tailleFen*0.4),(int)( tailleFen*0.4), (int)(tailleFen*0.20), (int)(tailleFen*0.20));
			
		  if (Panneau.PanneauCourant.fini==true){
			  g.drawString("Fichier test.txt créé", 75, 75);
		  }
	}
	
	}

	  

	  


