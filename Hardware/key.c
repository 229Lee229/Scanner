#include "My_include.h"


// 
extern bool MANUAL_FLAG;

void key_Init(void){
    GPIO_InitTypeDef GPIO_InitStructure;

    // ʹ��GPIOAʱ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    // ����PB3&PB4Ϊ��������
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOB, &GPIO_InitStructure);	
	
	
	
	
	// ����AFIO���ú�EXTI��
    EXTI_InitTypeDef EXTI_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    // ʹ��AFIOʱ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

    // ӳ�� PA3-EXTI3,PA4-EXTI4
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource3); // PA3   EXTI3
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource4); // PA4   EXTI4

    // ����EXTI3&EXTI4Ϊ�½��ش���
    EXTI_InitStructure.EXTI_Line = EXTI_Line3 | EXTI_Line4;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    // ����NVIC(�ж����ȼ�����ʵ����������)
    NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;
    NVIC_Init(&NVIC_InitStructure);	
	
	
}

// �жϷ�����
extern char Rx_scanner_temp[rx_buffer_scanner_MAX]; 
extern uchar rx_buffer_scanner[rx_buffer_scanner_MAX];  
extern uint16_t scanner_rx_index;
extern bool key1;
extern int blink_cnt;


uint8_t calculate_xor_checksum(uint8_t *data,uint8_t len){
	uint8_t checksum = 0;
	for(int i = 0; i < len;i++){
		checksum ^= data[i];
	}
	return checksum;
}


void EXTI3_IRQHandler(void) {
    if (EXTI_GetITStatus(EXTI_Line3) != RESET) {
		USART_ITConfig(USART2,USART_IT_RXNE,DISABLE);//??????

		
		
		if(MANUAL_FLAG == true){				// �ϵ�Ĭ���ֶ���־λ(���������־λ)Ϊ��
			TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE); 
#ifdef 	DEBUG_PRINT
			key1 = true;
#endif
			blink_cnt = 0;
			Send_Cmd_trig_Cmd();
			for(int i  = 0; i <= 5555;i++){
				;
			}
			Send_Cmd_trig_ON_Cmd();			
				
		}else{

		uint8_t frame[5];
		frame[0] = 0xAA;
		frame[1] = 0x01;
		frame[2] = 0x02;
		frame[3] = calculate_xor_checksum(frame,3);		// ���У��
		frame[4] = 0x0D;	
		for(uint8_t i= 0;i < 5;i++){
		USART_SendData(CH340_UART1,frame[i]);
			while(USART_GetFlagStatus(CH340_UART1, USART_FLAG_TXE) == RESET){
	#ifdef DEBUG_PRINT_2 			
				// printf("Send_Cmd_trig_Cmd_flag_wait...\r\n");
	#endif
			} // �ȴ��������;
		}	
			
		// printf("NG\r");
		
		}
		
//		memset(rx_buffer_scanner, 0, sizeof(rx_buffer_scanner));	
//		memset(Rx_scanner_temp, 0, sizeof(Rx_scanner_temp));	
//		scanner_rx_index = 0;
		USART_ITConfig(USART2,USART_IT_RXNE,ENABLE);//??????

        EXTI_ClearITPendingBit(EXTI_Line3); // ����жϱ�־
    }
}

// EXTI4??????
void EXTI4_IRQHandler(void) {
    if (EXTI_GetITStatus(EXTI_Line4) != RESET) {
		USART_ITConfig(USART2,USART_IT_RXNE,DISABLE);//??????

		// Send_Continuous_Cmd();

		uint8_t frame[5];
		frame[0] = 0xAA;    // ֡ͷ
		frame[1] = 0x01;	// ��������
		frame[2] = 0x01;	// ����ֵ
		frame[3] = calculate_xor_checksum(frame,3);		// ���У��
		frame[4] = 0x0D;	// CR
		for(uint8_t i= 0;i < 5;i++){
		USART_SendData(CH340_UART1,frame[i]);
			while(USART_GetFlagStatus(CH340_UART1, USART_FLAG_TXE) == RESET){
	#ifdef DEBUG_PRINT_2 			
				// printf("Send_Cmd_trig_Cmd_flag_wait...\r\n");
	#endif
			} // �ȴ��������;
		}			

		
		// printf("QC\r");

		USART_ITConfig(USART2,USART_IT_RXNE,ENABLE);//??????

        // ����PB4�½����ж�
        EXTI_ClearITPendingBit(EXTI_Line4); // ??????
    }
}

