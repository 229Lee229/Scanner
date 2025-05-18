#include "My_include.h"


volatile int blink_cnt = 0;
void LED_State_Init(void){
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOA,ENABLE);
	GPIO_InitTypeDef  GPIO_InitStructure;
	// ��ʼ��PA15 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);

	// ��ʼ��PB9
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	// ������ ��������˸
	GPIO_WriteBit(GPIOB,GPIO_Pin_9,Bit_SET);
	GPIO_WriteBit(GPIOA,GPIO_Pin_15,Bit_SET);
	
	
	// ��ʼ����ʱ��3 ���ڵ���ģʽ��ɨ�� LED��˸ ɨ��ɹ���ɨ��ʱ�䵽ֹͣLED
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    
    TIM_TimeBaseStructure.TIM_Period = 1800;         // ARR	
    TIM_TimeBaseStructure.TIM_Prescaler = 9200;        // PSC	
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
    
    TIM_ITConfig(TIM3, TIM_IT_Update, DISABLE);         // 
    TIM_Cmd(TIM3, ENABLE);                             // 	
	
	// ����NVIC
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
	
}
void TIM3_IRQHandler(void) {
    if (TIM_GetITStatus(TIM3, TIM_IT_Update) == SET) {
		
		blink_cnt++;
		PBout(9) = !PBout(9);
		if(blink_cnt == 70){
			PBout(9) = 1;
			TIM_ITConfig(TIM3, TIM_IT_Update, DISABLE); 
			blink_cnt = 0;
			
		}
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update); // ����жϱ�־λ
    }
}

