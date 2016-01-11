
import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Dimension;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.ItemEvent;
import java.awt.event.ItemListener;

import javax.swing.JButton;
import javax.swing.JComboBox;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JMenu;
import javax.swing.JMenuBar;
import javax.swing.JMenuItem;
import javax.swing.JPanel;


public class Fenetre extends JFrame implements ActionListener{ 
	public static Fenetre FenetreCourante;
	static Panel Principal=new Panel();
	  private JButton bouton = new JButton("Generer le code");
	  private JComboBox combo2 = new JComboBox();
	  private JLabel label2 = new JLabel("Secteur Radiaux: ");
	  private JComboBox combo = new JComboBox();
	  private JLabel label = new JLabel("NombreLED: ");



	
/*	private JMenuBar menuBar = new JMenuBar(); 
	private JMenu testcode = new JMenu("Generer le Code");
	  private JMenu testreset= new JMenu("Reset");
	  private JMenu testauto = new JMenu("Generation automatique");
	  private JMenuItem itemb = new JMenuItem("Bleu");
	  private JMenuItem itemv = new JMenuItem("Vert");
	  private JMenuItem itemr = new JMenuItem("Rouge");
	  private JMenuItem itembv = new JMenuItem("Bleu-Vert");
	  private JMenuItem itembr = new JMenuItem("Bleu-Rouge");
	  private JMenuItem itemvr = new JMenuItem("Vert-Rouge");
	  private JMenuItem itembrv = new JMenuItem("Bleu-Rouge-Vert");
  
*/
	public Fenetre(){
		this.setSize(Case.tailleFen, Case.tailleFen);
		this.setTitle("Choix du dessin");
		this.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		this.setLocationRelativeTo(null);
		this.setResizable(false);
		this.setBackground(Color.BLACK);
		this.setContentPane(Principal);   
		this.addMouseListener(new Moustener());
		Principal.add(bouton);
		
		bouton.addActionListener(this);
		combo.setPreferredSize(new Dimension(100, 20));
		JPanel top = new JPanel();
		Principal.add(top, BorderLayout.SOUTH);
		 top.add(label);
		top.add(combo); //LED
		 top.add(label2);
		top.add(combo2);//RADIO
		combo.addItem(3);
	    combo.addItem(5);
	    combo.addItem(8);
	    combo.addItem(10);
	    combo.addItemListener(new ItemState());
		

	    combo2.addItem(12);
	    combo2.addItem(24);
	    combo2.addItem(36);
	    combo2.addItem(48);
	    combo2.addItemListener(new ItemState2());
	    
/*		this.testauto.add(itemr);
		this.testauto.add(itemb);
		this.testauto.add(itemv);
		this.testauto.add(itembv);
		this.testauto.add(itembr);
		this.testauto.add(itemvr);
		this.testauto.add(itembrv);
		this.menuBar.add(testauto);
		this.menuBar.add(testreset);
		this.menuBar.add(testcode);
	    this.setJMenuBar(menuBar);*/
		
		this.setVisible(true);
		FenetreCourante=this;
	}
	class ItemState implements ItemListener{
	    public void itemStateChanged(ItemEvent e) {
	    	Panneau.creerPanneau((int) e.getItem(),Panneau.PanneauCourant.getAngle());
	    	Fenetre.Principal.repaint();
	    }               
	  }
	class ItemState2 implements ItemListener{

	    public void itemStateChanged(ItemEvent e2) {
	    	Panneau.creerPanneau(Panneau.PanneauCourant.getNombreLED(),(int) e2.getItem());
	    	Fenetre.Principal.repaint();
	    }               

	  }

	public static Fenetre getFenetreCourante() {
		return FenetreCourante;
	}

	public static void setFenetreCourante(Fenetre fenetreCourante) {
		FenetreCourante = fenetreCourante;
	}
	 public void actionPerformed(ActionEvent act) {
		 	Panneau.Generer();
		    }      

		}


