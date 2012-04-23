import gui.UserInterface;

import java.awt.EventQueue;

/**
 * CharacterAdderTest is a started class. Really simple.
 * Nothing special.
 * @author ultracold (NJU-EE-081180095)
 * @version 20120302
 *
 */
public class CharacterAdderTest {
	
	/**
	 * @param args
	 */
	public static void main(String[] args) {
		// TODO Auto-generated method stub
		EventQueue.invokeLater(new Runnable() {
			public void run() {
				UserInterface.paint();
			}
		});
	}

}
