package network;

import java.io.*;

/**
 * This class is used to package the data for the Embedded Controller
 * to recognize.
 * @author ultracold (NJU-EE-081180095)
 * @version 20120302
 */
public class DataPackaging {
	private String currentFilename;
	private FileInputStream fin;
	private BufferedInputStream bin;
	
	public DataPackaging(String filename) throws Exception {
		currentFilename = filename;
		fin = new FileInputStream(currentFilename);
		bin = new BufferedInputStream(fin);
	}
	
	public byte[] PackageFromFile(int type, int off, int len, int validLen) throws Exception {
		byte[] packageData = new byte[len];
		bin.read(packageData, 0, len);
		ByteArrayOutputStream os = new ByteArrayOutputStream();
		BufferedOutputStream bos = new BufferedOutputStream(os);
		bos.write('[');
		bos.write('P');
		bos.write(type);
		bos.write(off & 0xFF);
		bos.write((off >> 8) & 0xFF);
		bos.write(packageData, 0, validLen);
		bos.write(']');
		bos.flush();
		return os.toByteArray();
	}
	
	public static byte[] PackageFromCommand(int type, int colorOne, 
											int colotTwo, String data, 
											int startRow, int startCol) throws Exception {
		ByteArrayOutputStream os = new ByteArrayOutputStream();
		BufferedOutputStream bos = new BufferedOutputStream(os);
		bos.write('[');
		bos.write('D');
		// FULLSCR or SCROLL
		if((type & (1<<2)) != (1<<2)) {
			bos.write(0x00);
		}else {
			bos.write(0xFF);
		}
		// DOT16 or DOT24
		if((type & (1<<1)) != (1<<1)) {
			bos.write(0x00);
		}else {
			bos.write(0xFF);
		}
		// Border?
		if((type & (1<<0)) != (1<<0)) {
			bos.write(0x00);
		}else {
			bos.write(0xFF);
		}
		// Color
		bos.write(colorOne);
		bos.write(colotTwo);
		// Whatever the type is, we should also send the startRow and
		// startCol in order to simplify the MCU coding, though the 
		// below two byte will be omitted automatically when choosing
		// the scroll mode.
		bos.write(startRow);
		bos.write(startCol);
		
		byte[] databyte = data.getBytes();
		if((type & (1<<1)) != (1<<1)) {
			bos.write(databyte);
		}else {
			for(int count = 0;count < databyte.length;count++) {
				if((databyte[count] & 0x80) == 0) {
					bos.write(0xA3);
					int addNum = databyte[count] + 0x80;
					bos.write(addNum);
				}else{
					int addNum = databyte[count];
					bos.write(addNum);
				}
			}
		}
		bos.write(']');
		bos.flush();
		return os.toByteArray();
	}
	
	public static void main(String[] args) throws Exception {
		/*
		String filename = "E:\\Files and Documents\\TV\\Error.Detect.log";
		DataPackaging dpk = new DataPackaging(filename);
		byte[] a = dpk.PackageFromFile(1, 0, 10,10);
		UdpTransmission udp = new UdpTransmission();
		udp.clientTransmit(a);*/
		
	}
}
