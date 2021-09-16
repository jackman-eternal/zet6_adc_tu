/************************************************************************************
 * �ļ���  ��main.c
 * ����    ��        
 * Ӳ�����ӣ�
 * LCD1602:RS -> PA11; RW -> PA12; E -> PA15;
 *         D0~D7 -> PB3~PB10
 * �Ƕȴ�����ģ��: VCC -> 5V; GND -> GND; AO -> PA2;
 *
 * ���������������Ƕ�ֵҺ����ʾ(ADC1��PA2��DMA��ʽ��ȡ)��
             ���ڽ��ղ������õ��Ƕ�ֵ��������115200����
 *           ��������λ����ʾ�Ƕ�ֵ��

**********************************************************************************/
#include "stm32f10x.h"
#include "bsp_usart1.h"
#include <string.h>
#include "delay.h"
#include "math.h"
#include "bsp_adc.h"
#include "ds18b20.h"
#include "usart.h"

unsigned char AD_CHANNEL=0;
unsigned long PH_num=0,PU_V=0;
float PH_Value=0;
u8 ph_temp=0,tu_temp=0;
u16 ph_result=0,tu_result=0;
u16 adc_1,adc_2;
u16 adc_v_1,adc_v_2;
float compensationCoefficient=1.0;//�¶�У׼ϵ��
float compensationVolatge;
float kValue=1.0;

float TU=0.0;
float TU_value=0.0;
float TU_calibration=0.0;
float temp_data=0.0;
float K_Value=3047.19;

u8 ISendByte(unsigned char sla,unsigned char c);
u8 IRcvByte(unsigned char sla);	
u8 SPIx_ReadWriteByte(u8 TxData);

unsigned char  Tx[20];   //���߷��ͻ���


// ADC1ת���ĵ�ѹֵͨ��MDA��ʽ����SRAM
extern __IO uint16_t ADC_ConvertedValue;

// �ֲ����������ڱ���ת�������ĵ�ѹֵ 	 
float ADC_ConvertedValueLocal;  

/**************TDSֵ�ɼ�����***************/
void TU_Value_Conversion()
{
	TU =(float) ADC_ConvertedValue/4096*3.3; // ��ȡת����ADֵ
	
		TU_calibration=-0.0192*(temp_data/10-25)+TU;  
	    TU_value=-865.68*TU_calibration + K_Value;
	
		if(TU_value<=0){TU_value=0;}
		if(TU_value>=3000){TU_value=3000;}
	
		
		/*��ʾTDS*/
		Tx[0]=(int)(TU_value)/1000+'0';
		Tx[1]=(int)(TU_value)%1000/100+'0';	
		Tx[2]=(int)(TU_value)%100/10+'0';
		Tx[3]=(int)(TU_value)%10+'0';
		
//		LCD_printchar(0x5,0,Tx[0]);
//		LCD_printchar(0x6,0,Tx[1]);
//		LCD_printchar(0x7,0,Tx[2]);
//		LCD_printchar(0x8,0,Tx[3]);
}

void TEMP_Value_Conversion()
{
	  temp_data=DS18B20_Get_Temp();
	
	  Tx[4]=(int)(temp_data)%1000/100+'0';	
	  Tx[5]=(int)(temp_data)%100/10+'0';
	  Tx[6]='.';
	  Tx[7]=(int)(temp_data)%10+'0';
	
//	    LCD_printchar(0x5,1,Tx[4]);
//		LCD_printchar(0x6,1,Tx[5]);
//		LCD_printchar(0x7,1,Tx[6]);
//		LCD_printchar(0x8,1,Tx[7]);
}

/**
  * @brief  ������
  * @param  ��
  * @retval ��
  */
int main(void)
{	
	delay_init(); 
	DS18B20_Init();
    /* ����USART1 */
    USART1_Config();
    ADCx_Init();  
     
  while(1)
	{	
		TEMP_Value_Conversion();
		TU_Value_Conversion();
		printf("%s \r\n",Tx);
		delay_ms(1000);
	}	
}

