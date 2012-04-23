/**
 * This class is used to deal with the UDP transmission.
 * Including the transmit and receive function.
 *  
 */

package network;

import java.net.*;

/**
 * UdpTransmission is used to send and receive UDP packets to and
 * from microcontroller. Specifically, the embedded device's IP
 * address is certain, which can only be changed by writing it 
 * in the code.
 * @author ultracold (NJU-EE-081180095)
 * @version 20120305
 *
 */
public class UdpTransmission {
	public static final String SERVERIPADDR = "172.16.15.1";
	public static final String CLIENTIPADDR = "172.16.15.80";
	public static final int DSTPORT = 50123;
	public static final int SRCPORT = 50892;
	
	private DatagramSocket dSocket;
	
	public UdpTransmission() {
		try	{
			//InetAddress clientIP = InetAddress.getByName(CLIENTIPADDR);
			dSocket = new DatagramSocket(SRCPORT);
		}catch(Exception ex) {
			ex.printStackTrace();
		}
	}
	
	public void clientTransmit(byte[] packagedByte) throws Exception {
		byte[] sendData = new byte[1024];
		sendData = packagedByte;
		InetAddress serverIP = InetAddress.getByName(SERVERIPADDR);
		
		DatagramPacket sendPacket = new DatagramPacket(sendData, sendData.length,
														serverIP, DSTPORT);
		dSocket.send(sendPacket);
	}
	
	public String clientReceive() throws Exception {
		byte[] receiveData = new byte[1024];
		DatagramPacket receivePacket = new DatagramPacket(receiveData,
														receiveData.length);
		dSocket.receive(receivePacket);
		return new String(receivePacket.getData());
	}
	
	public static void main(String[] args) throws Exception {
		// TODO Auto-generated method stub
		UdpTransmission a = new UdpTransmission();
		String abcde = "Only for testing.";
		a.clientTransmit(abcde.getBytes());
	}
	
	public void close() {
		dSocket.close();
	}
}
