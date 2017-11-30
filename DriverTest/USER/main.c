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
 ����ʱ�䣺2017/03/20
 �ļ�˵�����ǳ�����Ƽ���˾�泵�����������
 
 ����log��
 2017/03/20  CANͨ���ȶ�
 2017/03/21  ��Ӵ��ڣ�ʱ��ϵͳ
 2017/04/24  ����������빦�ܣ�ʹ��TIM2
 
 

************************************************/


/************************************************

���������

        6����� 







************************************************/

 int main(void)
 {	 
	 int i=0;
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

	 TIM1_PWM_Init();
	 
	 
	  
	delay_ms(1000);//��ʱʱ�䲻�ܳ���1800������ʱ��Ҫ�����
	
		
	TIM2_Encoder_Init();
	TIM3_Int_Init();          //��ʱ��3����ϵͳʱ��,1ms����һ���ж�  
	 
	ptForkLift->s16speedwant = 0;    //ע����Ҫ���ٶ�ֻ����PID���ƺ����ⲿ���ã�������POSITION_PID��������
	
	ptForkLift->s16speed_p = 11.89;
	ptForkLift->s16speed_i = 0.3;
	ptForkLift->s16speed_d = 4;
	
	CAN_Mode_Init(CAN_SJW_1tq,CAN_BS2_4tq,CAN_BS1_7tq,12,CAN_Mode_Normal);//CAN��ʼ������ģʽ,������250Kbps    
	ptForkLift->Can_Txmessage.StdId=0x01;			   // ��׼��ʶ�� 
    ptForkLift->Can_Txmessage.ExtId=0x12;			   // ������չ��ʾ�� 
    ptForkLift->Can_Txmessage.IDE=CAN_Id_Standard;     // ��׼֡
    ptForkLift->Can_Txmessage.RTR=CAN_RTR_Data;		   // ����֡
    ptForkLift->Can_Txmessage.DLC=8;				   // Ҫ���͵����ݳ���   
	
	printf("system init ok \r\n");

	//ptForkLift->bDrection = BACK;
	ptForkLift->bDrection = FOWARD;
	ptForkLift->u16PWM=0;
		
	
	SetPwmDir(ptForkLift);

 	while(1)
	{
		
		
		if( i>=0 && i < MAX_PWM)
		{
			ptForkLift->bDrection = FOWARD;
			ptForkLift->u16PWM=i;
		}
		else if( i>=MAX_PWM && i < 2*MAX_PWM)
		{
			ptForkLift->bDrection = FOWARD;
			ptForkLift->u16PWM= 2*MAX_PWM - i;
		}
		else if( i>= 2*MAX_PWM && i < 3*MAX_PWM)
		{
			ptForkLift->bDrection = BACK;
			ptForkLift->u16PWM= i - 2*MAX_PWM;
		}
		else if( i>= 3*MAX_PWM && i < 4*MAX_PWM)
		{
			ptForkLift->bDrection = BACK;
			ptForkLift->u16PWM= 4*MAX_PWM - i;
		}
		SetPwmDir(ptForkLift);
		
		i++;
		
		if( i  == 4*MAX_PWM)
		{
			i = 0;
		}
		
		
		printf("EncoderSpeed=%d FilterSpeed=%d\r\n",ptForkLift->s16EncoderSpeed,ptForkLift->s16EncoderFilterSpeed);
        //ptForkLift->Can_Txmessage.Data[0] = 127+5;
		//Can_Send_Msg();
	}
}



