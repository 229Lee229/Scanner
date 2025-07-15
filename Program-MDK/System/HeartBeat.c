#include "My_include.h"



void HeartBeat_TIM_Init(void){
		// 初始化定时器3 来在单次模式中扫码 LED闪烁 扫码成功或扫码时间到停止LED
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    
    TIM_TimeBaseStructure.TIM_Period = 8899;         // ARR	
    TIM_TimeBaseStructure.TIM_Prescaler = 15999;        // PSC	
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
    
    TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);         // 
    TIM_Cmd(TIM4, ENABLE);                             // 	
	
	// 配置NVIC
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
	
	
	
	
	
}

extern bool data_REC_scanner_Flag;

void TIM4_IRQHandler(void) {
    if (TIM_GetITStatus(TIM4, TIM_IT_Update) == SET) {
		if(data_REC_scanner_Flag == true){
			TIM_ClearITPendingBit(TIM4, TIM_IT_Update); // 清除中断标志位
		}
		else{
		uint8_t frame[5];
		frame[0] = 0xAA;
		frame[1] = 0x11;
		frame[2] = 0x55;
		frame[3] = calculate_xor_checksum(frame,3);		// 异或校验
		frame[4] = 0x0D;	
		for(uint8_t i= 0;i < 5;i++){
		USART_SendData(CH340_UART1,frame[i]);
			while(USART_GetFlagStatus(CH340_UART1, USART_FLAG_TXE) == RESET){
	#ifdef DEBUG_PRINT_2 			
				// printf("Send_Cmd_trig_Cmd_flag_wait...\r\n");
	#endif
			} // 等待发送完成;
		}
		
        TIM_ClearITPendingBit(TIM4, TIM_IT_Update); // 清除中断标志位
		}
	}
}
