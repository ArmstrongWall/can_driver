#include "serial.h"
#include "control.h"
#include "can.h" 
#include "led.h"

/************************************************
 
 �����ߣ�����ǿ
 ����ʱ�䣺2017/03/20
 �ļ�˵�����ǳ�����Ƽ���˾�泵�����������
 
 ���ڿ��Ƴ����ڴ��ļ���
 
************************************************/

//////////////////////////////////////////////////////////////////
//�������´���,֧��printf����,������Ҫѡ��use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 


struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
_sys_exit(int x) 
{ 
	x = x; 
} 


	//�ض���fputc���� 
	int fputc(int ch, FILE *f)
	{      
		while((USART1->SR&0X40)==0);//ѭ������,ֱ���������   
		USART1->DR = (u8) ch;      
		return ch;
	}

#endif 

#if EN_USART1_RX   //���ʹ���˽���
//����1�жϷ������
//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���   	
u8 USART_RX_BUF[USART_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
//����״̬
//bit15��	������ɱ�־
//bit14��	���յ�0x0d
//bit13~0��	���յ�����Ч�ֽ���Ŀ
u16 USART_RX_STA=0;       //����״̬���
char state = 0;
void uart_init(u32 bound){
  //GPIO�˿�����
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//ʹ��USART1��GPIOAʱ��
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO,ENABLE);//ʹ��USART1,�����˿�B�͸��ù���ʱ��
  GPIO_PinRemapConfig(GPIO_Remap_USART1,ENABLE);//ʹ�ܶ˿���ӳ��
	
	//USART1_TX   GPIOB.6
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6; //PB.6
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
  GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��GPIOB.6
   
  //USART1_RX	  GPIOB.7��ʼ��
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;//PA10
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
  GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��GPIOA.10  

  //Usart1 NVIC ����
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
  
   //USART ��ʼ������

	USART_InitStructure.USART_BaudRate = bound;//���ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

  USART_Init(USART1, &USART_InitStructure); //��ʼ������1
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//�������ڽ����ж�
  USART_Cmd(USART1, ENABLE);                    //ʹ�ܴ���1 

}


/*
@��������

	һ֡���ݵĽṹ  �˸��ֽ�

		֡ͷ  һ�����ٶ�       �������ٶ�        �������ٶ�       ֡β
		0x53  0xFF    0xFF     0xFF    0xFF      0xFF 0xFF        0x45
			  ��λ    ��λ
��ӦCAN��Ϣ Data[0] Data[1]    Data[2] Data[3]   Data[4] Data[5]
	
	*/
bool start_flag = 0;
#define MESSAGE_SIZE 6

void USART1_IRQHandler(void)                	//����1�жϷ������
{
	
	u8 Res;
	u8 i;

	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x3b��β)
	{
		Res =USART_ReceiveData(USART1);	//��ȡ���յ�������
						
		
		
		if(start_flag)
		{
			en_queue(ptmessage_queue,Res);	
		}
		
		if(Res==0x53)//�յ���ʼ��S������
		{ 
			start_flag = 1;
			
		}
		if(Res==0x45)//�յ�������E������
		{ 
			start_flag = 0;
			de_queue(ptmessage_queue);
			
			for( i = 0 ; i < MESSAGE_SIZE ; i++ )
			{
				ptForkLift->Can_Txmessage.Data[i] = de_queue(ptmessage_queue);
			}
			Can_Send_Msg();
			
			ptForkLift->u16CanComCount[0]++;
			
		}
		
		
	
		
	}

} 
#endif	
