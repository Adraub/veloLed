import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.FileWriter;
import java.io.IOException;
import jssc.SerialPort;
import jssc.SerialPortException;


public class Panneau {
	int nombreLED;
	int angle;
	public static Panneau PanneauCourant;
	Case listeCase[];
	public boolean fini;
	
	public static void creerPanneau(int nombreLED, int angle) {
		Panneau pano= new Panneau(nombreLED, angle);
		PanneauCourant=pano;
		for(int i = 0; i < nombreLED; i++){    
		  for(int j = 0; j < angle; j++){
		    Case point = new Case(pano, j,i);
		    pano.listeCase[i*angle+j]=point;
		    System.out.println("enregistrement "+(i*angle+j)+", rayon:"+point.getRayon()+", angulaire:"+point.getSecteurAngulaire());
		  }  
		}
	}
	
	public Panneau(int nombreLED, int angle) {
		this.nombreLED = nombreLED;
		this.angle = angle;
		this.listeCase=new Case[nombreLED*angle];
		this.fini=false;
	}
	
	public static Panneau getPanneauCourant() {
		return PanneauCourant;
	}

	public static void setPanneauCourant(Panneau panneauCourant) {
		PanneauCourant = panneauCourant;
	}

	public Case[] getListeCase() {
		return listeCase;
	}

	public void setListeCase(Case[] listeCase) {
		this.listeCase = listeCase;
	}

	public int getNombreLED() {
		return nombreLED;
	}
	public void setNombreLED(int nombreLED) {
		this.nombreLED = nombreLED;
	}
	public int getAngle() {
		return angle;
	}
	public void setAngle(int angle) {
		this.angle = angle;
	}

	public static Case rechercheCase(int cercle, int secteurAngu) {
	System.out.println(" recherche de la case rayon "+cercle+", angu "+secteurAngu);
		for(int i = 0; i < PanneauCourant.listeCase.length; i++)
		{
			Case cas=PanneauCourant.listeCase[i];
			if(cas.getRayon()==cercle && cas.getSecteurAngulaire()==secteurAngu){
				System.out.println(" case trouvée! ");
				return cas;
			}
		}
		System.out.println("Case non trouvée");
		return null;
	}

	public static void Generer() {
		File f = new File ("test.txt");
		//int Binaire[] = new int[PanneauCourant.angle];
		byte Binaire[] = new byte[PanneauCourant.angle];
		 for(int i = 0; i < Binaire.length; i++){
			 Binaire[i]=0+(1<<2)+(1<<3)+1+(1<<4)+(1<<1);
		 }
	      try
	      {
	    	  SerialPort serialPort = new SerialPort("COM6");
	    	  try {
                  System.out.println("Port opened: " + serialPort.openPort());
                  System.out.println("Params setted: " + serialPort.setParams(9600, 8, 1, 0));
                  //System.out.println("\"Hello World!!!\" successfully writen to port: " + serialPort.writeBytes("Hello World!!!".getBytes()));
                  //System.out.println("Port closed: " + serialPort.closePort());
              }
              catch (SerialPortException ex){
                  System.out.println(ex);
              }
	    	  FileWriter fw = new FileWriter (f);
	          for(int i = 0; i < PanneauCourant.listeCase.length; i++)
		 		{	
		 			Case cas=PanneauCourant.listeCase[i];
		 			//int nombre=0;
		 			 	
		 			if(cas.isRouge()){
		 				System.out.println(String.format("%8s", Integer.toBinaryString(Binaire[cas.getSecteurAngulaire()] & 0xFF)).replace(' ', '0'));
			        	  System.out.println("  avant rouge  ");  
		 				Binaire[cas.getSecteurAngulaire()]=(byte) (Binaire[cas.getSecteurAngulaire()]|(1<<5));
		 				System.out.println(String.format("%8s", Integer.toBinaryString(Binaire[cas.getSecteurAngulaire()] & 0xFF)).replace(' ', '0'));
			        	  System.out.println("  apres rouge   "); 
		 			}
		 			if(cas.isBleu()){
		 				Binaire[cas.getSecteurAngulaire()]=(byte) (Binaire[cas.getSecteurAngulaire()]|(1<<7));
		 				System.out.println("  apres bleu   "); 
		 			}
		 			if(cas.isVert()){
		 				Binaire[cas.getSecteurAngulaire()]=(byte) (Binaire[cas.getSecteurAngulaire()]|(1<<6));
		 			}
		 			if(cas.isRouge()||cas.isBleu()||cas.isVert()){
		 				
		 				switch(cas.rayon){
			        	  case 4:
			        		  Binaire[cas.getSecteurAngulaire()]= (byte) (Binaire[cas.getSecteurAngulaire()]&(0xFE));
					 			
			        		  ;
			        		  break;
			        	  case 3:
			        		  Binaire[cas.getSecteurAngulaire()]= (byte) (Binaire[cas.getSecteurAngulaire()]&(0xFD));
					 			
			        		  break;
			        	  case 2:
			        		  Binaire[cas.getSecteurAngulaire()]= (byte) (Binaire[cas.getSecteurAngulaire()]&(0xFB));
					 			
			        		  break;
			        	  case 1:
			        		  Binaire[cas.getSecteurAngulaire()]= (byte) (Binaire[cas.getSecteurAngulaire()]&(0xF7));
					 			
			        		  break;
			        	  case 0:
			        		  Binaire[cas.getSecteurAngulaire()]= (byte) (Binaire[cas.getSecteurAngulaire()]&(0xEF));
					 			
			        		  break;
			        		  }
		 			}
		 			fw.write("LED de la ligne: "+cas.getRayon()+" du secteur: "+cas.getSecteurAngulaire()+" est à l'état: "+cas.isBleu()+" "+cas.isRouge()+" "+cas.isVert()+" .");
		 			/*if(cas.isRouge()||cas.isBleu()||cas.isVert()){
		 				if(cas.isRouge()&& ((Binaire[cas.getSecteurAngulaire()]&(0x20))!=0x20)){
		 					nombre=nombre+32;
		 				}
		 				if(cas.isBleu()&& ((Binaire[cas.getSecteurAngulaire()]&(0x80))!=0x80)){
		 					nombre=nombre+128;
		 				}
		 				if(cas.isVert()&& ((Binaire[cas.getSecteurAngulaire()]&(0x40))!=0x40)){
		 					nombre=nombre+64;
		 				}
		 				Binaire[cas.getSecteurAngulaire()]=(int) (Binaire[cas.getSecteurAngulaire()]-Math.pow(2, cas.getRayon())+nombre+31); 	
		 			}*/
		 			
		 			
		 			
		 			
		 			fw.write ("\r\n");		 						
		 			}
	          for(int i = 0; i < Binaire.length; i++){
	        	 /* System.out.println("boucle");
	        	  String str=Integer.toString(Binaire[i],2);
	        	  System.out.println();
	        	  System.out.println(str);
	        	  switch(str.length()){
	        	  case 0:
	        		  str="00000000";
	        		  break;
case 1:
	str="0000000"+str;
	        		  break;
case 2:
	str="000000"+str;
	  break;
case 3:
	str="00000"+str;
	  break;
case 4:
	str="0000"+str;
	  break;
case 5:
	str="000"+str;
	  break;
case 6:
	str="00"+str;
	  break;
case 7:
	str="0"+str;
	  break;
case 8:
	  
	  break;
	        	  }
	        	  System.out.println();
	        	  System.out.println("apres modif "+str);
	        	  */
	        	  
	        	 // Byte c=Byte.
	        			  //parseHex(Binaire[i], 2);
	        	  System.out.println(String.format("%8s", Integer.toBinaryString(Binaire[i] & 0xFF)).replace(' ', '0'));
	        	  System.out.println();
	        	  
	        	  try {
	        		  serialPort.writeBytes(Binaire);
	        	  }
	        	  catch (SerialPortException ex){
	                  System.out.println(ex);
	              }
	        	  fw.write(Binaire[i]);
	        			  //fw.write(" ");
	          
		 		}
		
		/*String separator = System.getProperty( "line.separator" );
		System.out.println("Generation");
	      FileOutputStream fos = null;
	      try {
	         fos = new FileOutputStream(new File("test.txt"));
	         String content="";
	 		for(int i = 0; i < PanneauCourant.listeCase.length; i++)
	 		{	
	 			String partial="";
	 			Case cas=PanneauCourant.listeCase[i];
	 			partial="LED de la ligne: "+cas.getRayon()+" du secteur: "+cas.getSecteurAngulaire()+" est à l'état: "+cas.isBleu()+" "+cas.isRouge()+" "+cas.isVert()+" .";
	 			content=partial+content;
	 			content=content+separator;
	 			
	 			
	 			*/
	 		
	         System.out.println("Copie terminée !");
	         Panneau.PanneauCourant.fini=true;
	         Fenetre.Principal.repaint();
	         fw.close();
	      }
	      catch (FileNotFoundException e) {
	         e.printStackTrace();
	      } catch (IOException e) {
 e.printStackTrace();

	
	
	      }
}
}
