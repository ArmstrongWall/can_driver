#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "lcd.h"
#include "usart.h"	 
#include "can.h" 
#include "timer.h" 
#include "control.h"
 
/************************************************
 ALIENTEKս��STM32������ʵ��26
 CANͨ��ʵ��
 ����֧�֣�www.openedv.com
 �Ա����̣�http://eboard.taobao.com 
 ��ע΢�Ź���ƽ̨΢�źţ�"����ԭ��"����ѻ�ȡSTM32���ϡ�
 ������������ӿƼ����޹�˾  
 ���ߣ�����ԭ�� @ALIENTEK
 
 ���ο�����
 �����ߣ�����ǿ
 ����ʱ�䣺2017/11/21
 �ļ�˵����˶ʿ��ҵ��ƴ���תCAN����
 
 ����log��

 
 

************************************************/


/************************************************

���������

        6����� 


************************************************/

void can_init(void);

 int main(void)
 {	 
	 ptForkLift=&TForkLift; 
	 ptmessage_queue = &message_queue;
	 create_queue(ptmessage_queue);   //����һ����Ϣ����
	 ptmessage_queue->size = 0;
	 
	 delay_init();
	 NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//�����ж����ȼ�����Ϊ��2��2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	
	 /*           ��ռ���ȼ�         �����ȼ�3��
	  CAN1             	0                  	2
	  
	  UART1            	3                  	3
	 
	 */
	 LED_Init();
	 uart_init(115200);	 	//���ڳ�ʼ��Ϊ115200
	 TIM1_PWM_Init();
	  
	 delay_ms(1000);//��ʱʱ�䲻�ܳ���1800������ʱ��Ҫ�����
		
	 TIM3_Int_Init();          //��ʱ��3����ϵͳʱ��,1ms����һ���ж�  
	 
	 can_init();	//can���߳�ʼ��
		
	 printf("This UART to CAN node\r\n");
	

 	while(1)
	{
//		printf("EncoderSpeed=%d FilterSpeed=%d\r\n",ptForkLift->s16EncoderSpeed,ptForkLift->s16EncoderFilterSpeed);
//        printf("speedout=%f ErrorSum=%f PWM=%d\r\n",ptForkLift->s16speedout,ptForkLift->s16ErrorSum,ptForkLift->u16PWM);
// 		printf("EncoderSpeed=%d FilterSpeed=%d\r\n",ptForkLift->s16EncoderSpeed,ptForkLift->s16EncoderFilterSpeed);
//			
		
//		printf("EncoderSpeed=%d FilterSpeed=%d\r\n",ptForkLift->s16EncoderSpeed,ptForkLift->s16EncoderFilterSpeed);
        		
		
//		printf_queue(ptmessage_queue);
//		printf("queue size = %d\r\n",ptmessage_queue->size);
//		
//		printf("\r\n");
	}
}



void can_init(void)
{
	CAN_Mode_Init(CAN_SJW_1tq,CAN_BS2_4tq,CAN_BS1_7tq,12,CAN_Mode_Normal);//CAN��ʼ������ģʽ,������250Kbps    
	ptForkLift->Can_Txmessage.StdId=0x01;			   // ��׼��ʶ�� 
    ptForkLift->Can_Txmessage.ExtId=0x12;			   // ������չ��ʾ�� 
    ptForkLift->Can_Txmessage.IDE=CAN_Id_Standard;     // ��׼֡
    ptForkLift->Can_Txmessage.RTR=CAN_RTR_Data;		   // ����֡
    ptForkLift->Can_Txmessage.DLC=8;				   // Ҫ���͵����ݳ���   
}


