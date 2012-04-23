`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date:    15:07:43 11/29/2010 
// Design Name: 
// Module Name:    vcxp-xc 
// Project Name: 
// Target Devices: 
// Tool versions: 
// Description: 
//
// Dependencies: 
//
// Revision: 
// Revision 0.01 - File Created
// Additional Comments: 
//
//////////////////////////////////////////////////////////////////////////////////
module vcxpxc(llc,HREF,VREF,YUV,DVD,
             sramAdr,sramData,sramrd,
             conce,conwr,conData,conAddr,
             LLCin,RTS0,RTCO,RTCI,HS,VS,RCV1,RCV2,ZERO);//,Out1,Out2,Out3,Out4,Out5,Out6,Out7);   

    input   [0:0]  llc,HREF,VREF,VS,HS,RTCO,ZERO,RTS0;
    output  [0:0]  RCV1,RCV2,LLCin,RTCI,sramrd;
    output  [18:0] sramAdr;
    inout   [7:0]  sramData;
       
    input   [1:0]  conAddr;
    input   [0:0]  conce,conwr;
    input   [7:0]  YUV ,conData;

    output [7:0]  DVD; 
    wire   clk1,wsramce,A0,llc1;
    reg [18:0] sramAdr1;
    reg [18:0] sramAdr2;
    reg [4:0]  sramAdr3;


    assign A0= sramAdr1[0:0];

    assign sramrd =  ~conce;


    assign LLCin = llc? 1: 0;
    assign RTCI  = ZERO ? 0: RTCO;
    assign RCV1  = ZERO ? VS: 0;
    assign RCV2  = ZERO ? HS: 0;

    assign llc1 = llc&HREF&VREF;   

    assign clk1 = (llc&HREF&VREF&HS&VS&YUV&RTS0);   
    assign sramData = sramrd ?  conData:8'bZ;

    assign sramAdr[18:0] = conce ? sramAdr1[18:0]:(sramAdr3+ sramAdr2[18:0]);//

    assign DVD  = (conce &(|sramData)) ? sramData: YUV;
 
 
    always@( posedge conwr  )//上升
       begin
              if(conce == 0)
              begin
                  if( conAddr == 1)
                     begin
                      sramAdr2[7:0] = conData[7:0];  //预置SRAM地址
                      sramAdr3= 0;

                     end 
                  else if ( conAddr == 2)
                      sramAdr2[15:8] = conData[7:0];  
                  else if ( conAddr == 3)
                      sramAdr2[18:16] = conData[2:0];  //
                  else
                      begin 
                          sramAdr3 = sramAdr3 +1;
                          if(sramAdr3 == 0)
                          begin         
                           sramAdr2[18:5] = sramAdr2[18:5]+45; 
                          end  
                      end 
              end 
	 end	    



    always@(posedge llc )//上升
       begin
          if(VREF ==  0) //场同步时清 SRAM地址计数器
               begin
                   sramAdr1 = 0;
               end
         else if (HREF == 0)
              begin
	      end	    
         else   
              begin   
						sramAdr1 = sramAdr1+1;
              end
       end

endmodule
