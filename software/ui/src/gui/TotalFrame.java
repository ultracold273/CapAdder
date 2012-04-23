package gui;

import java.awt.*;
import java.awt.event.*;
import java.io.*;
import javax.swing.*;

/**
 * TotalFrame is used to draw a display frame including the 
 * basic Graphics User Interface.
 * @author ultracold (NJU-EE-081180095)
 * @version 20120305
 */
public class TotalFrame extends JFrame {
	
	public final JFileChooser fileChooser;

	public TotalFrame() {
		Toolkit kit = Toolkit.getDefaultToolkit();
		Dimension screenSize = kit.getScreenSize();
		int screenWidth = screenSize.width;
		int screenHeight = screenSize.height;
		setSize(screenWidth / 2, screenHeight / 2);
		setTitle("�ַ�������");
		fileChooser = new JFileChooser();
		
		JPanel head = new JPanel();
		JLabel title = new JLabel("�ַ�������");
		title.setFont(new Font("SansSerif", Font.BOLD, 20));
		JButton change = new JButton("�л�����");
		head.add(title);
		head.add(change);
		final CardLayout cdl = new CardLayout();
		final JPanel body = new JPanel();
		body.setLayout(cdl);
		body.add(drawCommandPanel());
		body.add(drawDownloadPanel());
		change.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent e) {
				cdl.previous(body);
			}
		});
		add(head, BorderLayout.NORTH);
		add(body, BorderLayout.CENTER);
	}
	
	private JPanel drawCommandPanel() {
		JPanel panelOne = new JPanel();
		ButtonGroup displayMode = new ButtonGroup();
		final JRadioButton fullScrButton = new JRadioButton("ȫ��", false);
		JRadioButton scrollDisplay = new JRadioButton("����", false);
		displayMode.add(fullScrButton);
		panelOne.add(fullScrButton);
		displayMode.add(scrollDisplay);
		panelOne.add(scrollDisplay);
		
		JPanel panelTwo = new JPanel();
		ButtonGroup fontSize = new ButtonGroup();
		final JRadioButton dot16 = new JRadioButton("ʮ������", false);
		JRadioButton dot24 = new JRadioButton("��ʮ�ĵ���", false);
		fontSize.add(dot16);
		panelTwo.add(dot16);
		fontSize.add(dot24);
		panelTwo.add(dot24);
		
		JPanel panelThree = new JPanel();
		ButtonGroup border = new ButtonGroup();
		final JRadioButton yesBorder = new JRadioButton("�й���", false);
		JRadioButton noBorder = new JRadioButton("�޹���", false);
		border.add(yesBorder);
		panelThree.add(yesBorder);
		border.add(noBorder);
		panelThree.add(noBorder);
		
		JPanel panelFour = new JPanel();
		JLabel colorOne = new JLabel("��ɫһ:", JLabel.LEFT);
		final JTextField textColorOne = new JTextField("RED", 7);
		JLabel colorTwo = new JLabel("��ɫ��:", JLabel.LEFT);
		final JTextField textColorTwo = new JTextField("BLACK", 7);
		panelFour.add(colorOne);
		panelFour.add(textColorOne);
		panelFour.add(colorTwo);
		panelFour.add(textColorTwo);
				
		JPanel panelFive = new JPanel();
		JLabel row = new JLabel("��ʼ��:", JLabel.LEFT);
		final JTextField textRow = new JTextField("0", 7);
		JLabel col = new JLabel("��ʼ��:", JLabel.LEFT);
		final JTextField textCol = new JTextField("0", 7);
		panelFive.add(row);
		panelFive.add(textRow);
		panelFive.add(col);
		panelFive.add(textCol);
		
		JPanel panelSix = new JPanel();
		JLabel text = new JLabel("��ʾ�ı�", JLabel.LEFT);
		final JTextField textField = new JTextField("", 15);
		panelSix.add(text);
		panelSix.add(textField);
		
		JPanel panelSeven = new JPanel();
		final JButton updateButton = new JButton("����");
		// TODO: Here need to implement a inner-class
		updateButton.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent e) {
				int type = 0x00;
				if(!fullScrButton.isSelected()) {
					type |= (1<<2);
				}else {
					type &= ~(1<<2);
				}
				if(!dot16.isSelected()) {
					type |= (1<<1);
				}else {
					type &= ~(1<<1);
				}
				if(!yesBorder.isSelected()) {
					type |= (1<<0);
				}else {
					type &= ~(1<<0);
				}
				int cOne = UserInterface.String2Color(textColorOne.getText().trim());
				int cTwo = UserInterface.String2Color(textColorTwo.getText().trim());
				int sRow = Integer.parseInt(textRow.getText().trim());
				int sCol = Integer.parseInt(textCol.getText().trim());
				String sData = textField.getText().trim();
				UserInterface.sendCommand(type, cOne, cTwo, sData, sRow, sCol);
				JOptionPane.showConfirmDialog(null, "���óɹ�", "Complete", 
						JOptionPane.CLOSED_OPTION, JOptionPane.INFORMATION_MESSAGE);
			}
		});
		panelSeven.add(updateButton);
				
		JPanel sumOne = new JPanel();
		sumOne.setLayout(new GridLayout(7,1));
		sumOne.add(panelOne);
		sumOne.add(panelTwo);
		sumOne.add(panelThree);
		sumOne.add(panelFour);
		sumOne.add(panelFive);
		sumOne.add(panelSix);
		sumOne.add(panelSeven);
		
		return sumOne;
	}
	
	private JPanel drawDownloadPanel() {
		JPanel panelAlpha = new JPanel();
		ButtonGroup displayType = new ButtonGroup();
		final JRadioButton AscChara = new JRadioButton("ASC�ַ�", false);
		final JRadioButton ChiChara = new JRadioButton("�����ַ�", false);
		displayType.add(AscChara);
		panelAlpha.add(AscChara);
		displayType.add(ChiChara);
		panelAlpha.add(ChiChara);
		
		JPanel panelBeta = new JPanel();
		ButtonGroup fontSizeBeta = new ButtonGroup();
		final JRadioButton dot16Beta = new JRadioButton("ʮ������", false);
		final JRadioButton dot24Beta = new JRadioButton("��ʮ�ĵ���", false);
		fontSizeBeta.add(dot16Beta);
		panelBeta.add(dot16Beta);
		fontSizeBeta.add(dot24Beta);
		panelBeta.add(dot24Beta);
		
		JPanel panelGamma = new JPanel();
		JLabel openAt = new JLabel("��", JLabel.LEFT);
		final JTextField fileLocation = new JTextField("", 20);
		JButton open = new JButton("...");
		open.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent e) {
				fileChooser.setCurrentDirectory(new File("."));
				int result = fileChooser.showOpenDialog(null);
				if(result == JFileChooser.APPROVE_OPTION) {
					String filename = fileChooser.getSelectedFile().getAbsolutePath();
					fileLocation.setText(filename);
				}else {
					fileLocation.setText("");
				}
			}
		});
		panelGamma.add(openAt);
		panelGamma.add(fileLocation);
		panelGamma.add(open);
		
		JPanel panelDelta = new JPanel();
		JButton updateBase = new JButton("�����ֿ�");
		// TODO: null should be replace by an inner class
		updateBase.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent e) {
				String filename = fileLocation.getText().trim();
				int type = 0;
				if(AscChara.isSelected()) {
					if(!dot16Beta.isSelected()) {
						type = 2;
					}else {
						type = 1;
					}
				}else if(ChiChara.isSelected()) {
					if(!dot16Beta.isSelected()) {
						type = 4;
					}else {
						type = 3;
					}
				}
				UserInterface.burnCharacter(filename, type);
				JOptionPane.showConfirmDialog(null, "��д�ɹ�", "Complete", 
						JOptionPane.CLOSED_OPTION, JOptionPane.INFORMATION_MESSAGE);
			}
		});
		panelDelta.add(updateBase);
		
		JPanel sumTwo = new JPanel();
		sumTwo.setLayout(new GridLayout(4,1));
		sumTwo.add(panelAlpha);
		sumTwo.add(panelBeta);
		sumTwo.add(panelGamma);
		sumTwo.add(panelDelta);
		
		return sumTwo;
	}
	
	/**
	 * In order to throw out the warning.
	 */
	private static final long serialVersionUID = -3148471420694481334L;
}
