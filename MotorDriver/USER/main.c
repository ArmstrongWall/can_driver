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
 �����ߣ�����ǿ
 ����ʱ�䣺2017/03/20
 �ļ�˵����ͬ�ô�ѧ����ǿSLAM���ܳ������������
 
 ����log��
 2017/03/20  CANͨ���ȶ�
 2017/03/21  ��Ӵ��ڣ�ʱ��ϵͳ
 2017/04/24  ����������빦�ܣ�ʹ��TIM2
 2017/12/20  ʹ�ú궨������ǵ�������幤�����Ǹ�ģʽ����"control.h"����
************************************************/


/************************************************

���������

       3����� 
			 
		0�ǽӵأ������� ON KE������(������GND����)
		
		2�Ű���   1�Ű���
			PB1     PB10
			 0       0     0����
			 0       1     1����
			 1       0     2����	



************************************************/
void can_init(void);


 int main(void)
 {	 
	
	 #if OPENLOOPTESTMODE	|| CLOSELOOPTESTMODE
			int i=0;
	 #endif
	 ptForkLift=&TForkLift; 
	 ptmiddle_filter_queue = &middle_filter_queue;
	 create_queue(ptmiddle_filter_queue);   //����һ���������˲�����
	 
	 delay_init();
	 NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//�����ж����ȼ�����Ϊ��2��2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	
	 /*           ��ռ���ȼ�         �����ȼ�3��
	  CAN1             	0                  	2
	  
	  TIM1 			   	0				  	3       PWM���
	  TIM2			   	1					0		��������
	  UART1            	3                  	3
	 
	 */
	 LED_Init();
	 uart_init(115200);	 	//���ڳ�ʼ��Ϊ115200
	 
	 delay_ms(1000);//��ʱʱ�䲻�ܳ���1800������ʱ��Ҫ�����
	 printf("This is wheel slave\r\n");
	 
	 CarID_Select();

	 TIM1_PWM_Init();
	 
	 
	  
	delay_ms(1000);//��ʱʱ�䲻�ܳ���1800������ʱ��Ҫ�����
	
		
	TIM2_Encoder_Init();
	TIM3_Int_Init();          //��ʱ��3����ϵͳʱ��,1ms����һ���ж�  
	 
	ptForkLift->s16speedwant = 0;    //ע����Ҫ���ٶ�ֻ����PID���ƺ����ⲿ���ã�������POSITION_PID��������
	
//		��ɫ�ذ峵PID����	
//		ptForkLift->s16speed_p = 11.89;
//		ptForkLift->s16speed_i = 0.3;
//		ptForkLift->s16speed_d = 4;

	ptForkLift->s16speed_p = 0.7;
	ptForkLift->s16speed_i = 0.2;
	ptForkLift->s16speed_d = 4;
	
	can_init();
		
	


 	while(1)
	{
		#if OPENLOOPTESTMODE		
				if( i>=0 && i < MAX_PWM)
				{
					ptForkLift->s16speedwant= i;
				}
				else if( i>=MAX_PWM && i < 2*MAX_PWM)
				{
					ptForkLift->s16speedwant= 2*MAX_PWM - i;
				}
				else if( i>= 2*MAX_PWM && i < 3*MAX_PWM)
				{
					ptForkLift->s16speedwant = 2*MAX_PWM - i;
				}
				else if( i>= 3*MAX_PWM && i < 4*MAX_PWM)
				{
					ptForkLift->s16speedwant=  i- 4*MAX_PWM ;
				}
								
				i++;
				delay_ms(10);
				
				if( i  == 4*MAX_PWM)
				{
					i = 0;
				}
		#endif
				
		#if CLOSELOOPTESTMODE		
				if( i>=0 && i < MAX_PLUSE)
				{
					ptForkLift->s16speedwant= i;
				}
				else if( i>=MAX_PLUSE && i < 2*MAX_PLUSE)
				{
					ptForkLift->s16speedwant= 2*MAX_PLUSE - i;
				}
				else if( i>= 2*MAX_PLUSE && i < 3*MAX_PLUSE)
				{
					ptForkLift->s16speedwant = 2*MAX_PLUSE - i;
				}
				else if( i>= 3*MAX_PLUSE && i < 4*MAX_PLUSE)
				{
					ptForkLift->s16speedwant=  i- 4*MAX_PLUSE ;
				}
								
				i++;
				delay_ms(10);
				
				if( i  == 4*MAX_PLUSE)
				{
					i = 0;
				}
		#endif	
		
		//printf("EncoderSpeed=%d FilterSpeed=%d\r\n",ptForkLift->s16EncoderSpeed,ptForkLift->s16EncoderFilterSpeed);
    //ptForkLift->Can_Txmessage.Data[0] = 127+5;
		//Can_Send_Msg();
		//printf("ptForkLift->s16speedwant=%d\r\n",i);
    
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

