#include "My_include.h"

// unsigned rx_buffer_scanner_MAX = 2048;
uchar rx_buffer_scanner[rx_buffer_scanner_MAX];  
char Rx_scanner_temp[rx_buffer_scanner_MAX]; 

uchar rx_buffer_ch340[rx_buffer_scanner_MAX];
char Rx_ch340_temp[rx_buffer_scanner_MAX];


uint16_t scanner_rx_index = 0;
uint16_t ch340_rx_index = 0;
bool data_REC_scanner_Flag;
bool data_REC_ch340_Flag;

extern char *AT_CONTINUOUS_Str;
extern char *AT_MANUAL_Str;
extern char *AT_ReadDeviceID_Str;
char *Bootloader_Str = "BOOT\r";
volatile uint8_t Manual_Cmd[] = {0x7E,0x00,0x08,0x01,0x00,0x00,0xD4,0xAB,0xCD};
volatile uint8_t Continuous_Cmd[] = {0x7E,0x00,0x08,0x01,0x00,0x00,0xD6,0xAB,0xCD};
volatile uint8_t Single_time_25s_Cmd[] = {0x7E,0x00,0x08,0x01,0x00,0x06,0xFE,0xAB,0xCD};
volatile uint8_t Signle_time_15s_Cmd[] = {0x7E,0x00,0x08,0x01,0x00,0x06,0x9B,0xAB,0xCD};
// volatile uint8_t Cmd_trig_Cmd[] = {0x7E,0x00,0x08,0x01,0x00,0x00,0xD5,0xAB,0xCD};
volatile uint8_t Cmd_trig_Cmd[] = {0x7E,0x00,0x08,0x01,0x00,0x00,0xD5,0xEF,0x41};

// volatile uint8_t Cmd_trig_ON_Cmd[] = {0x7E,0x00,0x08,0x01,0x00,0x02,0x01,0xAB,0xCD};
volatile uint8_t Cmd_trig_ON_Cmd[] = {0x7E,0x00,0x08,0x01,0x00,0x02,0x01,0x02,0xDA};

volatile char *ReadDeviceId = "ReadDeviceID.";

extern bool Scan_Data_Flag;

extern volatile int blink_cnt;
extern bool MANUAL_FLAG;

// ���������ģʽָ��
void Send_Cmd_trig_ON_Cmd(void){
	for(uint16_t i= 0;i < 9;i++){
		USART_SendData(SCANNER_UART2,Cmd_trig_ON_Cmd[i]);
		while(USART_GetFlagStatus(SCANNER_UART2, USART_FLAG_TXE) == RESET){
#ifdef DEBUG_PRINT_2
			printf("Send_Cmd_trig_ON_flag_wait...\r\n");
#endif			
		} // �ȴ��������;
	}
}


// �����ֶ�ģʽָ��
void Send_Manual_Cmd(void){
	for(uint16_t i= 0;i < 9;i++){
		USART_SendData(SCANNER_UART2,Manual_Cmd[i]);
		while(USART_GetFlagStatus(SCANNER_UART2, USART_FLAG_TXE) == RESET){
#ifdef DEBUG_PRINT
			printf("Send_Manual_flag_wait...\r\n");
#endif			
		} // �ȴ��������;
	}
}

// ��������ģʽָ��
void Send_Continuous_Cmd(void){
	for(uint16_t i= 0;i < 9;i++){
		USART_SendData(SCANNER_UART2,Continuous_Cmd[i]);
		while(USART_GetFlagStatus(SCANNER_UART2, USART_FLAG_TXE) == RESET){
#ifdef DEBUG_PRINT_2 			
			printf("Send_Continuous_flag_wait...\r\n");
#endif
		} // �ȴ��������;
	}
}

// ���Ͷ�ȡscannerIDָ��   SN MID FID
void Send_ReadDeviceID_Cmd(void){
	for (uint16_t i = 0; ReadDeviceId[i] != '\0'; i ++)
	{
		USART_SendData(SCANNER_UART2,ReadDeviceId[i]);
		while(USART_GetFlagStatus(SCANNER_UART2, USART_FLAG_TXE) == RESET){
#ifdef DEBUG_PRINT 				
			// printf("Send_ReadID_flag_wait...\r\n");
#endif			
		} // �ȴ��������;
	}
	
}

// �������õ����Զ�ʱ��15s
void Send_Single_Cmd(int second){
	
	switch(second){
		case 25:
				for(uint16_t i= 0;i < 9;i++){
					USART_SendData(SCANNER_UART2,Single_time_25s_Cmd[i]);
					while(USART_GetFlagStatus(SCANNER_UART2, USART_FLAG_TXE) == RESET){
			#ifdef DEBUG_PRINT 			
						printf("Send_Single_time_25s_Cmd_flag_wait...\r\n");
			#endif
					} // �ȴ��������;
				}	
				break;
		case 15:
				for(uint16_t i= 0;i < 9;i++){
					USART_SendData(SCANNER_UART2,Signle_time_15s_Cmd[i]);
					while(USART_GetFlagStatus(SCANNER_UART2, USART_FLAG_TXE) == RESET){
			#ifdef DEBUG_PRINT 			
						printf("Send_Single_time_15s_Cmd_flag_wait...\r\n");
			#endif
					} // �ȴ��������;
				}				
			break;
//		case 5:
//				for(uint16_t i= 0;i < 9;i++){
//					// USART_SendData(SCANNER_UART2,Signle_time_5s_Cmd[i]);
//					while(USART_GetFlagStatus(SCANNER_UART2, USART_FLAG_TXE) == RESET){
//			#ifdef DEBUG_PRINT 			
//						printf("Send_Single_time_5s_Cmd_flag_wait...\r\n");
//			#endif
//					} // �ȴ��������;
//				}				
//				break;
		default:
				for(uint16_t i= 0;i < 9;i++){
					USART_SendData(SCANNER_UART2,Signle_time_15s_Cmd[i]);
					while(USART_GetFlagStatus(SCANNER_UART2, USART_FLAG_TXE) == RESET){
			#ifdef DEBUG_PRINT 			
						printf("Send_Single_time_15s_Cmd_flag_wait...\r\n");
			#endif
					} // �ȴ��������;
				}	
	}
}

// �������������ģʽ(����˵���ֶ�����,��������,MCU���������)
void Send_Cmd_trig_Cmd(void){
	for(uint16_t i= 0;i < 9;i++){
		USART_SendData(SCANNER_UART2,Cmd_trig_Cmd[i]);
		while(USART_GetFlagStatus(SCANNER_UART2, USART_FLAG_TXE) == RESET){
#ifdef DEBUG_PRINT_2 			
			printf("Send_Cmd_trig_Cmd_flag_wait...\r\n");
#endif
		} // �ȴ��������;
	}	
	
	
}


void Handle_RX_Scanner(void){   		// ������յ���scanner����,�������ݴ���usart1
			// USART_Cmd(USART2,DISABLE);// �ȴ�����������ʹ�ܴ���
#ifdef DEBUG_PRINT			
			// printf("Identification successful!\r\n");
#endif
	
	
			if(Scan_Data_Flag == true){
				char Scan_Data[1048];
				
				if(MANUAL_FLAG == false){				
					// �����������ɨ��ģʽ,ɨ��������������5min��ʱ��			
									// ????????????
//				TIM1->CNT = 0;          // ���ü�����
//				TIM1->EGR |= TIM_EGR_UG; // �����ظ�������																																																										



					// ??????
					TIM1->DIER &= ~TIM_DIER_UIE;

					// ??????
					TIM1->CNT = 0;

					// ??????
					TIM1->EGR |= TIM_EGR_UG;

					// ??????
					TIM1->SR &= ~TIM_SR_UIF;

					// ????????
					TIM1->DIER |= TIM_DIER_UIE;					
				}
				
				sprintf(Scan_Data,"A:%s",(char *)Rx_scanner_temp);// CODE_DATA:
				Delay_us(1);
				printf("%s",(char *)Scan_Data);
				// ɨ��ɹ�, ��״̬��
				TIM_ITConfig(TIM3, TIM_IT_Update, DISABLE); 
				if(MANUAL_FLAG == true){				// �������ֶ�ģʽ��, ɨ��ɹ�״̬��Ϩ��, ��������ģʽ��, ��״̬�Ƴ���
					PBout(9) = 1;
					blink_cnt = 0;
				}
				

				
				
				memset(rx_buffer_scanner, 0, sizeof(rx_buffer_scanner));	
				memset(Rx_scanner_temp, 0, sizeof(Rx_scanner_temp));	
				scanner_rx_index = 0;
				
				data_REC_scanner_Flag = false;		
				Scan_Data_Flag = false;				
				return;
			}
			printf("%s",(char *)Rx_scanner_temp);			// ������ʲô���  ��һ�Ƿ���ID
			// ��ӡ�����ݺ� ����������ݺ���������
			memset(rx_buffer_scanner, 0, sizeof(rx_buffer_scanner));	
			memset(Rx_scanner_temp, 0, sizeof(Rx_scanner_temp));	
			scanner_rx_index = 0;
			
			data_REC_scanner_Flag = false;
			// USART_Cmd(USART2,ENABLE);// �Ѵ������� ʹ�ܴ���	
	
}

extern volatile bool sleep_flag;
void Handle_RX_CH340(void){
	// AT_CONTINUOUS �˶��Ƿ���ȷ
	if(strcmp(Rx_ch340_temp,AT_CONTINUOUS_Str) == 0){			// AT_CONTINUOUS����˶���ȷ,����ָ��
		// ����ָ��
		Send_Continuous_Cmd();
		sleep_flag = false;
		TIM1->DIER &= ~TIM_DIER_UIE;

		// ??????
		TIM1->CNT = 0;

		// ??????
		TIM1->EGR |= TIM_EGR_UG;

		// ??????
		TIM1->SR &= ~TIM_SR_UIF;

		// ????????
		TIM1->DIER |= TIM_DIER_UIE;	
		TIM_Cmd(TIM1, ENABLE);			// ��ʼ��ʱ5min, ��ɨ����������װ
		
		MANUAL_FLAG = false;				// ���յ�����ģʽ��ָ���, �ֶ�ģʽ��ʧ��, �����ָ�����NGָ��
#ifdef	DEBUG_PRINT
		printf("CONTINUOUS_MATCH!\r\n");
#endif
		
		// ��ƥ��ɹ�,�򷵻�Ӧ����Ϣ
		uint8_t frame[5];
		frame[0] = 0xAA;
		frame[1] = 0x11;
		frame[2] = 0xAB;
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
//		PAout(15) = 0;
//		PBout(9) = 1;
		
		Delay_ms(100);
			memset(rx_buffer_scanner, 0, sizeof(rx_buffer_scanner));	
			memset(Rx_scanner_temp, 0, sizeof(Rx_scanner_temp));	
			scanner_rx_index = 0;
			
			data_REC_scanner_Flag = false;
			TIM_ITConfig(TIM3, TIM_IT_Update, DISABLE); 
			blink_cnt = 0;
			PBout(9) = 0;

	}
	// AT_MANUAL �˶��Ƿ���ȷ
	else if(strcmp(Rx_ch340_temp,AT_MANUAL_Str) == 0){
		// ����ָ��
		// Send_Manual_Cmd();
		Send_Cmd_trig_Cmd();
		
		// �л��ֶ�ģʽʱ,��TIM1�ر�
		TIM1->DIER &= ~TIM_DIER_UIE;

		// ??????
		TIM1->CNT = 0;

		// ??????
		TIM1->EGR |= TIM_EGR_UG;

		// ??????
		TIM1->SR &= ~TIM_SR_UIF;

		// ????????
		TIM1->DIER |= TIM_DIER_UIE;			

		TIM_Cmd(TIM1,DISABLE);
		//-----------------------------------------
		for(int i  = 0; i <= 5555;i++){
			;
		}
		MANUAL_FLAG = true;				// ���յ��ֶ�ģʽ��ָ���, �ֶ�ģʽ��ʹ��, �����ָ���ع���
		PBout(9) = 1;					// �ֶ�ģʽ״̬�Ʋ�����, ��Ϊ������������˸
	
		// Send_Cmd_trig_ON_Cmd();
#ifdef	DEBUG_PRINT
		printf("MANUAL_MATCH!\r\n");
#endif
		// ��ƥ��ɹ�,�򷵻�Ӧ����Ϣ
		uint8_t frame[5];
		frame[0] = 0xAA;
		frame[1] = 0x11;
		frame[2] = 0xAB;
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
//		PBout(9) = 0;
//		PAout(15) = 1;
				Delay_ms(100);
			memset(rx_buffer_scanner, 0, sizeof(rx_buffer_scanner));	
			memset(Rx_scanner_temp, 0, sizeof(Rx_scanner_temp));	
			scanner_rx_index = 0;
			
			data_REC_scanner_Flag = false;
	}
	
	// AT_ReadDeviceID_Str �˶��Ƿ���ȷ
	else if(strcmp(Rx_ch340_temp,AT_ReadDeviceID_Str) == 0){
		Send_ReadDeviceID_Cmd();
		// printf("ID_MATCH!\r\n");		
		for(int i = 0;i <= 5555;i++){
			;
		}
		Delay_ms(100);
		Delay_ms(100);

	}
	// ����bootloaderģʽ
	else if(strcmp(Rx_ch340_temp,Bootloader_Str) == 0){
		// 1.д2C00��־λ��
		Write_App_Version();
		// д������
		NVIC_SystemReset();
	}
	
	
	else{	
		uint8_t frame[5];
		frame[0] = 0xAA;
		frame[1] = 0x11;
		frame[2] = 0xFF;
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
	
#ifdef DEBUG_PRINT_3
	printf(":%s.",rx_buffer_ch340);
	
		
	// printf("NOT_MATCH\r\n");
#endif
	}
	memset(rx_buffer_ch340, 0, sizeof(rx_buffer_ch340));	
	memset(Rx_ch340_temp, 0, sizeof(Rx_ch340_temp));
	ch340_rx_index = 0;
	data_REC_ch340_Flag = false;
}

extern bool key1;

void USART1_IRQHandler(void){
	if(USART_GetITStatus(CH340_UART1,USART_IT_RXNE) != RESET){
		uint8_t Rx_temp = USART_ReceiveData(CH340_UART1);
		if(ch340_rx_index >= sizeof(rx_buffer_ch340)){
            ch340_rx_index = 0; //��ֹ���ڱ�ˢ��			
		}
		rx_buffer_ch340[ch340_rx_index++] = Rx_temp;		
		if(ch340_rx_index >= 2 && (rx_buffer_ch340[ch340_rx_index-1] == '\r')/* && rx_buffer_scanner[scanner_rx_index-1] == '\n'*/ ){			// �ռ������ַ�����
			strcpy(Rx_ch340_temp,(char *)rx_buffer_ch340); // ��������Ƶ��м����
			data_REC_ch340_Flag = true;
		}

		USART_ClearITPendingBit(CH340_UART1, USART_IT_RXNE);
	}
	
}


void USART2_IRQHandler(void) {
    if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) {
        uint8_t Rx_temp = USART_ReceiveData(USART2);
//		if(key1 == true){
//			printf("\r\n%c\r\n:",Rx_temp);
//			
//		}
		if(scanner_rx_index >= sizeof(rx_buffer_scanner))
            scanner_rx_index = 0; //��ֹ���ڱ�ˢ��
		
		rx_buffer_scanner[scanner_rx_index++] = Rx_temp;
		// ����ָ��
		if(rx_buffer_scanner[scanner_rx_index-7] == 0x02 \
			&&rx_buffer_scanner[scanner_rx_index-6] == 0x00 &&rx_buffer_scanner[scanner_rx_index-5] == 0x00 \
			&& rx_buffer_scanner[scanner_rx_index-4] == 0x01 &&rx_buffer_scanner[scanner_rx_index-3] == 0x00 \
			&&rx_buffer_scanner[scanner_rx_index-2] == 0x33 && rx_buffer_scanner[scanner_rx_index-1] == 0x31){
			// ������ָ����ȷ,��ɾ���û����ָ��,����Ӱ�������������
			memset(rx_buffer_scanner, 0, sizeof(rx_buffer_scanner));	
			memset(Rx_scanner_temp, 0, sizeof(Rx_scanner_temp));	
			scanner_rx_index = 0;
		}
		// ��������
		else if(scanner_rx_index >= 2 && ((rx_buffer_scanner[scanner_rx_index-1] == '\r') || (rx_buffer_scanner[scanner_rx_index-1] == '.'))/* && rx_buffer_scanner[scanner_rx_index-1] == '\n'*/ ){			// �ռ������ַ�����
			strcpy(Rx_scanner_temp,(char *)rx_buffer_scanner); // ��������Ƶ��м����
			if((rx_buffer_scanner[scanner_rx_index-1] == '\r') == true){
				Scan_Data_Flag = true;
			}
			
			data_REC_scanner_Flag = true;

#ifdef   DEBUG_PRINT			
			PBout(9) = 1;
			TIM_ITConfig(TIM3, TIM_IT_Update, DISABLE); 
			blink_cnt = 0;
			key1 = false;
#endif			
		}
		USART_ClearITPendingBit(USART2, USART_IT_RXNE);
	}
}

//void USART2_IRQHandler(void) {
//    if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) {
//        char Rx_c = USART_ReceiveData(USART2);
//		if(scanner_rx_index >= sizeof(rx_buffer_scanner))
//            scanner_rx_index = 0; //��ֹ���ڱ�ˢ��
//		
//		rx_buffer_scanner[scanner_rx_index++] = Rx_c;
//		if(rx_buffer_scanner[scanner_rx_index-1] == '\r'){			// �ռ������ַ�����
//			strcpy(Rx_scanner_temp,(char *)rx_buffer_scanner); // ��������Ƶ��м����
//			data_REC_scanner_Flag = true;
//		}
//		USART_ClearITPendingBit(USART2, USART_IT_RXNE);
//	}
//}
