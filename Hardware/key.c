#include "My_include.h"


// 
extern bool MANUAL_FLAG;


// TIM2 检测双击时间窗口 (300ms)


volatile uint8_t pressCount = 0;		// 按键释放次数计数
volatile uint8_t click_event = 0;		// 单机事件标志
volatile uint8_t double_click_event = 0;// 双击事件标志
void TIM1_Init(void) {
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    // ??TIM1??
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);

    // ???????
    TIM_TimeBaseStructure.TIM_Prescaler = 36000 - 1;   // 72MHz / 7200 = 10kHz(0.1ms??)   36000
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_Period = 40000 - 1;        // 100 * 0.1ms = 1ms   40000
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 14;   // ???????????		20s*15=300s=5min
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

    // ??TIM1????
	TIM_ClearFlag(TIM1, TIM_FLAG_Update); // ??????

    TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);
    TIM_Cmd(TIM1, DISABLE);

    // ??TIM1?????
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

    NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn;  // TIM1??????
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    // ??TIM1(?????)
    
}

void TIM_Double_Click_Init(void){
	
	
	// 初始化定时器3 来在单次模式中扫码 LED闪烁 扫码成功或扫码时间到停止LED
	// RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    
    TIM_TimeBaseStructure.TIM_Period = 4000-1;         // ARR	
    TIM_TimeBaseStructure.TIM_Prescaler = 7200 - 1;        // PSC	
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
    
	
	TIM_ClearFlag(TIM2, TIM_FLAG_Update); // ??????
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);         // 
    TIM_Cmd(TIM2, DISABLE);                             // 	
	
	// 配置NVIC
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
	
}

	
void key_Init(void){
    GPIO_InitTypeDef GPIO_InitStructure;

    // 使能GPIOA时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    // 配置PB3&PB4为浮空输入
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOB, &GPIO_InitStructure);	
	
	
	
	
	// 配置AFIO复用和EXTI线
    EXTI_InitTypeDef EXTI_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    // 使能AFIO时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

    // 映射 PA3-EXTI3,PA4-EXTI4
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource3); // PA3   EXTI3
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource4); // PA4   EXTI4

    // 配置EXTI3&EXTI4为下降沿触发
    EXTI_InitStructure.EXTI_Line = EXTI_Line3 | EXTI_Line4;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    // 配置NVIC(中断优先级根据实际需求设置)
    NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;
			NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;
    NVIC_Init(&NVIC_InitStructure);	
	
	
}

// 中断服务函数
extern char Rx_scanner_temp[rx_buffer_scanner_MAX]; 
extern uchar rx_buffer_scanner[rx_buffer_scanner_MAX];  
extern uint16_t scanner_rx_index;
extern bool key1;
extern int blink_cnt;

extern volatile bool sleep_flag;
uint8_t calculate_xor_checksum(uint8_t *data,uint8_t len){
	uint8_t checksum = 0;
	for(int i = 0; i < len;i++){
		checksum ^= data[i];
	}
	return checksum;
}
void TIM1_UP_IRQHandler(void) {
    if (TIM_GetITStatus(TIM1, TIM_IT_Update) != RESET) {
        TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
		TIM_Cmd(TIM1, DISABLE);
		sleep_flag = true;
		Send_Cmd_trig_Cmd();
		for(int i  = 0; i <= 1555;i++){
			;
		}
	
//		
//		pressCount++;
//		if(pressCount == 1){
//			// TIM_SetAutoreload(TIM2,10000 - 1);
//			// TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
//		TIM_Cmd(TIM2, ENABLE);                             // 	
//		// TIM_SetCounter(TIM2,0);

//			// TIM_Cmd(TIM2, ENABLE);
//		}
//		 EXTI->IMR |= EXTI_Line3;    // ????????

		// printf("TIM1 test\r\n");
    }
}
void TIM2_IRQHandler(void){
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET) {
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
        // TIM_ITConfig(TIM2, TIM_IT_Update, DISABLE); 
		TIM_Cmd(TIM2, DISABLE);                             // 	

        // ???????????
        if (pressCount >= 2) {
            double_click_event = 1;
        } else if (pressCount == 1) {
            click_event = 1;
        }
        pressCount = 0; // 重置计数
    }
	
}

void EXTI3_IRQHandler(void) {
    if (EXTI_GetITStatus(EXTI_Line3) != RESET) {
		        EXTI_ClearITPendingBit(EXTI_Line3); // 清除中断标志

		// USART_ITConfig(USART2,USART_IT_RXNE,DISABLE);//??????
		EXTI->IMR &= ~EXTI_Line3;		// 禁用外部中断
//		pressCount++;
//		if(pressCount == 1){
//			// TIM_SetAutoreload(TIM2,10000 - 1);
//			TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
//						// TIM_SetCounter(TIM2,0);

//			// TIM_Cmd(TIM2, ENABLE);
//		}
		
		if(sleep_flag == true){
			sleep_flag = false;
			// printf("test key2\r\n");
			Send_Continuous_Cmd();
			TIM_Cmd(TIM1, ENABLE);
			EXTI->IMR |= EXTI_Line3;    // 开启外部中断

			return;
		}
		
		
		// 消抖
		for(int i = 50000;i>1;i--){
			;
		}
		pressCount++;
		if(pressCount == 1){
			// TIM_SetAutoreload(TIM2,10000 - 1);
			// TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
			TIM_Cmd(TIM2, ENABLE);                             // 	
		// TIM_SetCounter(TIM2,0);

			// TIM_Cmd(TIM2, ENABLE);
		}
		 EXTI->IMR |= EXTI_Line3;    // 开启外部中断
		// TIM_Cmd(TIM1, ENABLE);
		// TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);
		


		
        // }
        
		// USART_ITConfig(USART2,USART_IT_RXNE,ENABLE);//??????

    }
}


//void EXTI3_IRQHandler(void) {
//    if (EXTI_GetITStatus(EXTI_Line3) != RESET) {
//		USART_ITConfig(USART2,USART_IT_RXNE,DISABLE);//??????

//		
//		
//		if(MANUAL_FLAG == true){				// 上电默认手动标志位(即命令触发标志位)为真
//			TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE); 
//#ifdef 	DEBUG_PRINT
//			key1 = true;
//#endif
//			blink_cnt = 0;
//			Send_Cmd_trig_Cmd();
//			for(int i  = 0; i <= 5555;i++){
//				;
//			}
//			Send_Cmd_trig_ON_Cmd();			
//				
//		} // else{

//		uint8_t frame[5];
//		frame[0] = 0xAA;
//		frame[1] = 0x01;
//		frame[2] = 0x02;
//		frame[3] = calculate_xor_checksum(frame,3);		// 异或校验
//		frame[4] = 0x0D;	
//		for(uint8_t i= 0;i < 5;i++){
//		USART_SendData(CH340_UART1,frame[i]);
//			while(USART_GetFlagStatus(CH340_UART1, USART_FLAG_TXE) == RESET){
//	#ifdef DEBUG_PRINT_2 			
//				// printf("Send_Cmd_trig_Cmd_flag_wait...\r\n");
//	#endif
//			} // 等待发送完成;
//		}	
//			
//		// printf("NG\r");
//		
//		// }
//		
////		memset(rx_buffer_scanner, 0, sizeof(rx_buffer_scanner));	
////		memset(Rx_scanner_temp, 0, sizeof(Rx_scanner_temp));	
////		scanner_rx_index = 0;
//		USART_ITConfig(USART2,USART_IT_RXNE,ENABLE);//??????

//        EXTI_ClearITPendingBit(EXTI_Line3); // 清除中断标志
//    }
//}

// EXTI4??????
void EXTI4_IRQHandler(void) {
    if (EXTI_GetITStatus(EXTI_Line4) != RESET) {
		USART_ITConfig(USART2,USART_IT_RXNE,DISABLE);//??????
        EXTI_ClearITPendingBit(EXTI_Line4); // ??????

		
		if(sleep_flag == true){
			sleep_flag = false;
			// printf("test key1\r\n");	
			Send_Continuous_Cmd();
			USART_ITConfig(USART2,USART_IT_RXNE,ENABLE);//??????
			TIM_Cmd(TIM1, ENABLE);
			return;
		}		
		// TIM_Cmd(TIM1, ENABLE);
		// Send_Continuous_Cmd();

		uint8_t frame[5];
		frame[0] = 0xAA;    // 帧头
		frame[1] = 0x01;	// 按键类型
		frame[2] = 0x01;	// 按键值
		frame[3] = calculate_xor_checksum(frame,3);		// 异或校验
		frame[4] = 0x0D;	// CR
		for(uint8_t i= 0;i < 5;i++){
		USART_SendData(CH340_UART1,frame[i]);
			while(USART_GetFlagStatus(CH340_UART1, USART_FLAG_TXE) == RESET){
	#ifdef DEBUG_PRINT_2 			
				// printf("Send_Cmd_trig_Cmd_flag_wait...\r\n");
	#endif
			} // 等待发送完成;
		}			

		
		// printf("QC\r");

		USART_ITConfig(USART2,USART_IT_RXNE,ENABLE);//??????

        // 处理PB4下降沿中断
        // EXTI_ClearITPendingBit(EXTI_Line4); // ??????
    }
}

