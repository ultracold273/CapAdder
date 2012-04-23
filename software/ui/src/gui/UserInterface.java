package gui;

import javax.swing.JFrame;
import java.io.File;

import network.DataPackaging;
import network.UdpTransmission;

/**
 * UserInterface contains some static method interacting with
 * other classes, like manipulate the data into the data package.
 * @author ultracold (NJU-EE-081180095)
 * @version 20120305
 */
public class UserInterface {
	
	public static void paint() {
		TotalFrame myCharacterAdder = new TotalFrame();
		myCharacterAdder.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		myCharacterAdder.setVisible(true);
	}
	
	public static int String2Color(String s) {
		int colorCode;
		switch(s) {
		case "WHITE" : colorCode = 0; break;
		case "YELLOW" : colorCode = 1; break;
		case "MAGENTA" : colorCode = 2; break;
		case "RED" : colorCode = 3; break;
		case "CYAN" : colorCode = 6; break;
		case "GREEN" : colorCode = 7; break;
		case "BLUE" : colorCode = 8; break;
		case "BLACK" : colorCode = 10; break;
		default: colorCode = 0;break;
		}
		return colorCode;
	}
	
	public static void sendCommand(int type, int colorOne, 
								int colotTwo, String data, 
								int startRow, int startCol) {
		UdpTransmission tranSocket = new UdpTransmission();
		try {
			byte[] pkg = DataPackaging.PackageFromCommand(type, colorOne, 
					colotTwo, data, startRow, startCol);
			tranSocket.clientTransmit(pkg);
			while(true) {
				String receive = tranSocket.clientReceive().trim();
				if(receive.equals("OK")) {
					break;
				}
			}
		}catch(Exception ex) {
			ex.printStackTrace();
		}
		tranSocket.close();
	}
	
	public static void burnCharacter(String name, int type) {
		UdpTransmission tranSocket = new UdpTransmission();
		//name = name.replace(new String("\\"), new String("\\\\"));
		int[] lengthPerWord = new int[4];
		lengthPerWord[0] = 40;		// ASC16
		lengthPerWord[1] = 0;		// ASC24
		lengthPerWord[2] = 128;		// HZK16
		lengthPerWord[3] = 256;		// HZK24
		int[] validLength = new int[4];
		validLength[0] = 40;		// ASC16
		validLength[1] = 40;		// ASC24
		validLength[2] = 72;		// HZK16
		validLength[3] = 156;		// HZK24
		try{
			long fileLength = new File(name).length();
			int totalNumber = (int) fileLength / lengthPerWord[type - 1];
			DataPackaging aMachine = new DataPackaging(name);
			for(int i = 0;i < totalNumber;i++) {
				tranSocket.clientTransmit(
					aMachine.PackageFromFile(
						type, i, lengthPerWord[type - 1], validLength[type - 1]
						//type, 0, validLength[type - 1]
					)
				);
				
				String as = tranSocket.clientReceive().trim();
				
				if(as.equals("OK")) {
					;
				}else {
					return;
				}
				
			}
			tranSocket.close();
		}catch(Exception ex) {
			ex.printStackTrace();
		}	
	}
		
}
