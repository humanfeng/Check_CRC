/*************************************************************
;  Copyright (C), 2014, CQUPT.
;  FileName:      Check_CRC
;  Description:   Check_CRC  
;  Author:        youboFeng
;  Usage:         void Check_CRC( )  
;  Input:         int * RxPBCHSeqOut
                  int DeCRC_Type
                  int RxDeRateMatch_outputlen[]
                  int DeCRC_Scramble
                  int PBCH_DeCRC_mask
                  int * DeCRC_Outdatalen
                  int *DeCRC_Flag                    	              
;  Return:        NO
;  Altered Registers:    
************************************************************/
#include "math.h"   
void Check_CRC ( int * RxPBCHSeqOut,int DeCRC_Type,int RxDeRateMatch_outputlen[],int DeCRC_Scramble,int PBCH_DeCRC_mask,int * DeCRC_Outdatalen,int *DeCRC_Flag)
   {
	 int CrcPoly;
	 int temp0, temp1;
	 int word;
	 int i,j,n,m=0;
	 int crc=0;
	 int * DeCRC_outdata;

    /****************************************************************************************
   	                                     CRC类型选择
				           LTE只用到了CRC8，CRC16，CRC24A，CRC24B
     ****************************************************************************************/

     switch(DeCRC_Type)
     {
       case 1:
	     {
	 	  CrcPoly = 0xCD800000;  //CrcPoly =[1 1 0 0  1 1 0 1  1];
          break;
	     }
       case 2:
         {
		  CrcPoly = 0x88108000;	//CrcPoly =[1 0 0 0  1 0 0 0  0 0 0 1  0 0 0 0  1];
          break;
		 }
       case 3:
         {
	 	  CrcPoly = 0xC3267D80; //CrcPoly =[1 1 0 0  0 0 1 1  0 0 1 0  0 1 1 0  0 1 1 1  1 1 0 1  1]; 
          break;
		 }
       case 4:
         {
		  CrcPoly = 0xC0003180;//CrcPoly =[1 1 0 0  0 0 0 0  0 0 0 0  0 0 0 0  0 0 1 1  0 0 0 1  1];
          break; 
		 }
       default:
    	  break;
      }
   
   /**************************************************************************************
   	                                   如果有加扰,则先进行解扰
    ***************************************************************************************/

     if(DeCRC_Scramble == 1)
	 {
      temp0=PBCH_DeCRC_mask & 0x0000ff00;
	  temp0 >>= 8;
	  *(RxPBCHSeqOut) ^= temp0;
      temp1=PBCH_DeCRC_mask & 0x000000ff;
	  *(RxPBCHSeqOut+1) >>= 24;
      *(RxPBCHSeqOut+1) ^= temp1;
	  *(RxPBCHSeqOut+1) = (*(RxPBCHSeqOut+1)<<24) & 0xff000000;
	 }
   
    /************************************************************************************
				          判断解CRC输入数据占多少个字（32位为一个处理单元）
    *************************************************************************************/

       word=(int)RxDeRateMatch_outputlen/32;    //判断有几个字
	   if((int)RxDeRateMatch_outputlen % 32 != 0) //判断是否还有剩余比特
	   {
	     word= word+1;
	   }

   /*************************************************************************************
   	                                      校验CRC
   **************************************************************************************/
        DeCRC= RxPBCHSeqOut[0];
		
	    for(i=0;i<word;i++)
		{
			 for(j=0;j<4;j++)
			 {
		      
			        for(k=0;k<8;k++)
					{
                      if(DeCRC & 0x80000000)
					  {
			           DeCRC^= CrcPoly;
					   DeCRC<<= 1;
					  }
                      else
					  {
		               DeCRC<<= 1;   
					  } 
					} 
                 if(RxPBCHSeqOut[i+1] != 0)
				 {
			      temp=RxPBCHSeqOut[i+1] & 0xff000000;
			      RxPBCHSeqOut[i+1] <<= 8;
		  	      DeCRC ^=  temp;          
				 }
			 }     
		}
	   
   
        	if(DeCRC == 0)
	           DeCRC_Flag[0] = 1;
            else
               DeCRC_Flag[0] = 0;
}