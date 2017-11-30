#include "timer.h"
#include "led.h"
#include "usart.h"
#include "control.h"
#include "can.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEKս��STM32������
//��ʱ�� ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/9/4
//�汾��V1.1
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
//********************************************************************************
//V1.1 20120904
//1,����TIM3_PWM_Init������
//2,����LED0_PWM_VAL�궨�壬����TIM3_CH2����									  
//////////////////////////////////////////////////////////////////////////////////  
   	  
//ͨ�ö�ʱ��3�жϳ�ʼ��
//����ʱ��ѡ��ΪAPB1��2������APB1Ϊ36M
//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶ��
//����ʹ�õ��Ƕ�ʱ��3!
#define ENCODER_TIMER   TIM2  // Encoder unit connected to TIM2
u8 canbuf[8]={1},count=0;

static int queue_lenth =0;

void TIM3_Int_Init(void)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //ʱ��ʹ��
	
	/*
	
	ʱ���ж���72000000/71+1/1000=1000 HZ
	
	��1 ms
	  
	*/	
	//��ʱ��TIM3��ʼ��
	TIM_TimeBaseStructure.TIM_Period = 1000; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	
	TIM_TimeBaseStructure.TIM_Prescaler =71; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
 
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE ); //ʹ��ָ����TIM3�ж�,��������ж�

	//�ж����ȼ�NVIC����
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //��ռ���ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //�����ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  //��ʼ��NVIC�Ĵ���


	TIM_Cmd(TIM3, ENABLE);  //ʹ��TIMx					 
}
//��ʱ��3�жϷ������
void TIM3_IRQHandler(void)   //TIM3�ж�
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)  //���TIM3�����жϷ������
		{
		    TIM_ClearITPendingBit(TIM3, TIM_IT_Update  );  //���TIMx�����жϱ�־ 
		    
			
			ptForkLift->u16RunPIDControl++;
			ptForkLift->u16EncoderPeriod++;
			ptForkLift->u16RunLED++;
			
			if(ptForkLift->u16RunLED == 500)
			{
				LED0=!LED0;//��ʾϵͳ��������
				ptForkLift->u16RunLED=0;
			}
			
			
			/*����CAN����״̬������״̬���ж�*/
			ptForkLift->u16CanComPeriod++;
			
			if(ptForkLift->u16CanComPeriod==100)
			{
				ptForkLift->u16CanComCount[1]=ptForkLift->u16CanComCount[0];
			}
			
			else if(ptForkLift->u16CanComPeriod==500)
			{
			  
				if(ptForkLift->u16CanComCount[0]>ptForkLift->u16CanComCount[1])
					ptForkLift->bCanComBox = CanBoxPost;
				else
					ptForkLift->bCanComBox = CanBoxPend;
				
				ptForkLift->u16CanComPeriod=0;
				ptForkLift->u16CanComCount[1]=ptForkLift->u16CanComCount[0]=0;
							
			}
			
		
			/*�ٶȲɼ�*/	
			if(ptForkLift->u16EncoderPeriod == 9)//10ms����һ�βɼ����������ٶ�
			{
				ptForkLift->s16EncoderSpeed=ENCODER_TIMER->CNT-15000;//�ɼ����������ٶ�
				ENCODER_TIMER->CNT = 15000;
				
				ptForkLift->u16EncoderPeriod = 0;
			}
			
			
			
			
            /*�������*/			
			if(ptForkLift->u16RunPIDControl == 9)//20ms����һ��PID����
			{
				
				if(queue_lenth<9){
					queue_lenth++;
					en_queue(ptmiddle_filter_queue,ptForkLift->s16EncoderSpeed);	
					 
				}
				else 
				{
					en_queue(ptmiddle_filter_queue,ptForkLift->s16EncoderSpeed);	
					de_queue(ptmiddle_filter_queue);	
				 
				}
				
								
				ptForkLift->s16EncoderFilterSpeed = average_data(ptmiddle_filter_queue);
				
				//�ж�CAN�Ƿ�����
				if(ptForkLift->bCanComBox == CanBoxPost)
				{
					POSITION_PID(ptForkLift,ptForkLift->s16EncoderFilterSpeed);
				}
				
				else if(ptForkLift->bCanComBox == CanBoxPend)
				{
					ptForkLift->u16PWM = 0;
					SetPwmDir(ptForkLift);
				}
				
				
			    
				
				ptForkLift->u16RunPIDControl = 0;
				
			}
			
						
		}
}



//TIM1 PWM���ֳ�ʼ�� 
//PWM�����ʼ��
//arr���Զ���װֵ
//psc��ʱ��Ԥ��Ƶ��
void TIM1_PWM_Init()
{  
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);	//ʹ�ܶ�ʱ��1ʱ��
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA  | RCC_APB2Periph_AFIO, ENABLE);  //ʹ��GPIO�����AFIO���ù���ģ��ʱ��
	
	 
   //���ø�����Ϊ�����������,���TIM1 CH1��PWM���岨��	GPIOA.8
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9; //TIM_CH1
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //�����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIO
	
	
	
	/*
	PWMƵ��=                    ϵͳʱ��                          72000000 
	            ������������������������������������������  = ������������������������=20 KHz
					��TIM_Period+1��*��TIM_Prescaler+1��           1800*2
	
	
	*/
	
   //��ʼ��TIM1
	TIM_TimeBaseStructure.TIM_Period = 1799; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ
	TIM_TimeBaseStructure.TIM_Prescaler =1; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ 
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0; 
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
	
	TIM_ARRPreloadConfig(TIM1, ENABLE);//����ARRӰ�ӼĴ���
	
	//��ʼ��TIM1 Channel PWMģʽ	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ2
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //�Ƚ����ʹ��
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable; 
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //�������:TIM����Ƚϼ��Ը�
	TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High; 
	TIM_OCInitStructure.TIM_Pulse = 0;//��ʼռ�ձ�ΪTIM_Pulse/TIM_Period
	
	TIM_OC1Init(TIM1, &TIM_OCInitStructure);  //����Tָ���Ĳ�����ʼ������TIM1 OC1
	TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);  //ʹ��TIM3��CCR2�ϵ�Ԥװ�ؼĴ���
	
	
	TIM_OCInitStructure.TIM_Pulse = 0;//��ʼռ�ձ�ΪTIM_Pulse/TIM_Period
	TIM_OC2Init(TIM1, &TIM_OCInitStructure);  //����Tָ���Ĳ�����ʼ������TIM1 OC1
	TIM_OC2PreloadConfig(TIM1, TIM_OCPreload_Enable);  //ʹ��TIM3��CCR2�ϵ�Ԥװ�ؼĴ���
 
 
	TIM_Cmd(TIM1, ENABLE);  //ʹ��TIM1
	TIM_CtrlPWMOutputs(TIM1, ENABLE);//ʹ��PWM���
	
}

//TIM2 ���������ʼ��
void TIM2_Encoder_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_ICInitTypeDef TIM_ICInitStructure; 
	NVIC_InitTypeDef NVIC_InitStructure;
	
	    //PA0 ch1  ,PA1 ch2 
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);//ʹ��TIM2ʱ��  
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);//ʹ��GPIOAʱ��

    GPIO_StructInit(&GPIO_InitStructure);//��GPIO_InitStruct�еĲ�����ȱʡֵ����
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;         
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//PA0 PA1��������  
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);  
	
	
	/* �ж��������ʼ�� Enable the TIM3 Update Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
	
	
	TIM_DeInit(ENCODER_TIMER);
    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
	
	//��Ƶ����
	TIM_TimeBaseStructure.TIM_Prescaler = 0x0;  // No prescaling 
    TIM_TimeBaseStructure.TIM_Period = 30000;  
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;   
    TIM_TimeBaseInit(ENCODER_TIMER, &TIM_TimeBaseStructure);
	
	
	TIM_EncoderInterfaceConfig(ENCODER_TIMER, TIM_EncoderMode_TI12, 
                             TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
    TIM_ICStructInit(&TIM_ICInitStructure);
    TIM_ICInitStructure.TIM_ICFilter = 0;//ѡ������Ƚ��˲���
    TIM_ICInit(ENCODER_TIMER, &TIM_ICInitStructure);
  
    // Clear all pending interrupts û���жϷ������Դ˴���ע��
    //TIM_ClearFlag(ENCODER_TIMER, TIM_FLAG_Update);
    //TIM_ITConfig(ENCODER_TIMER, TIM_IT_Update, ENABLE);//�ж�
	
    //Reset counter
    
	TIM_SetCounter(ENCODER_TIMER,15000);
	TIM_Cmd(ENCODER_TIMER, ENABLE);
	
	
	
}





