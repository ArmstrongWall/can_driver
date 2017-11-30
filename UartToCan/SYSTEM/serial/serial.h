#ifndef __SERIAL_H
#define __SERIAL_H

#include "stm32f10x.h"
#include "stdio.h"	
#include "sys.h" 
#include <stdbool.h>

#define USART_REC_LEN  			5  	//�����������ֽ��� 200
#define EN_USART1_RX 			1		//ʹ�ܣ�1��/��ֹ��0������1����
	 


extern u8  USART_RX_BUF[USART_REC_LEN]; //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
extern u16 USART_RX_STA;         		//����״̬���	
extern bool start_flag;
//����봮���жϽ��գ��벻Ҫע�����º궨��

void uart_init(u32 bound);

#endif
