#include "led.h"
#include "control.h"
#include "usart.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEKս��STM32������
//LED��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/9/2
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	   

//��ʼ��PB5��PE5Ϊ�����.��ʹ���������ڵ�ʱ��		    
//LED IO��ʼ��
void LED_Init(void)
{
 
 
	 GPIO_InitTypeDef  GPIO_InitStructure;
		
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //ʹ��PB,PE�˿�ʱ��
		
	 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;				 //LED0-->PB.5 �˿�����
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
	 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
	 GPIO_Init(GPIOB, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOB.5
	 GPIO_SetBits(GPIOB,GPIO_Pin_12);						 //PB.5 �����


}


void CarID_Select(void)
{
     GPIO_InitTypeDef  GPIO_InitStructure;
		
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);//ʹ��PORTA,PORTEʱ��	
	 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_1;				 //LED0-->PB.5 �˿�����
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 		 //�������
	
	 GPIO_Init(GPIOB, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOB.5
	 
	/*  0�ǽӵأ������� ON KE������
			PB1   PB10
			 0     0     1����
			 0     1     2����
			 1     0     3����	
	*/
	 
	 //ptForkLift->u8CarID=GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_10);
	
	 if( GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_10)==0 && GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_1) == 0)
	 {
			ptForkLift->u8CarID = 0x00;
	 }
	 else if( GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_10)==1 && GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_1) == 0)
	 {
			ptForkLift->u8CarID = 0x01;
	 }
	 else if( GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_10)==0 && GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_1) == 1)
	 {
			ptForkLift->u8CarID = 0x02;
	 }
	 
	 printf("this is wheel %d\r\n",ptForkLift->u8CarID);
	 if(Mode == 1)
		 printf("��������ģʽ\r\n");
	 else if(Mode == 2)
		 printf("�ջ�����ģʽ\r\n");
	 else if(Mode == 3)
		 printf("��������ģʽ\r\n");
	 else if(Mode == 4)
		 printf("�ջ�����ģʽ\r\n");
	 
	 
}










 
