/**********************************************************************
 * flie:  main
 * brief:  
 * Input:  
 * Output: 
 * Retval: 
 *
 * History--------------------------------------------------------------
 * Version       Date         Name    			Changes and comments
 *=====================================================================
	V1.0		 4/9/2025    Lee				增加在连续模式下,yellow led常量;在手动下,yellow60闪烁不变	
	V1.1		 4/9/2025	 Lee				更改扫码数据发送格式,'DATA_CODE:'改成'A:'
	V1.2		 5/3/2025 	 Lee				在单次触发模式下,按下触发键(右键),扫码器开始扫描并且发送质检不合格指令
	V2.0		 5/4/2025	 Lee				增加串口接收Boot指令,进行重启通过串口烧程序
 ***********************************************************************/


#include "stm32f10x.h"                  // Device header
#include "My_include.h"

// 接收缓存
extern char Rx_scanner_temp[rx_buffer_scanner_MAX]; 
extern uchar rx_buffer_scanner[rx_buffer_scanner_MAX];  
extern char Rx_ch340_temp[rx_buffer_scanner_MAX];
extern uchar rx_buffer_ch340[rx_buffer_scanner_MAX];

// 接收字符串索引
extern uint16_t scanner_rx_index;
extern uint16_t ch340_rx_index;

// 接收标志位
extern bool data_REC_scanner_Flag;
extern bool data_REC_ch340_Flag;

// 手动标志位(默认开启)
bool MANUAL_FLAG = true;

bool key1 = false;

// 扫码数据标志位
bool Scan_Data_Flag = false;


extern volatile uint8_t click_event;		// 单机事件标志
extern volatile uint8_t double_click_event;// 双击事件标志
extern volatile int blink_cnt;


volatile bool  sleep_flag = false;

int main(void){
	/* 4/4 新增 */
//	RCC_DeInit();		// 复位时钟配置
//	// 开启HSE并等待就绪
//	RCC_HSEConfig(RCC_HSE_OFF);
//	while(RCC_GetFlagStatus(RCC_FLAG_HSIRDY) == RESET);
//	
//	// 配置PLL: HSE不分频, 倍频9倍
//	RCC_PLLConfig(RCC_PLLSource_HSI_Div2,RCC_PLLMul_9);
//	
//	// 开启PLL并等待就绪
//	RCC_PLLCmd(ENABLE);'
//	while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);
//	// 切换系统时钟源为PLL
////	RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
//	while (RCC_GetSYSCLKSource() != 0x00); // 确认切换成功

//	// 更新系统时钟全局变量
//	SystemCoreClockUpdate(); // 此变量在代码中用于跟踪主频	
	
	SystemInit();
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);			// 取消PB3 PB4 PA15的JTAG模式 改为普通IO
	TIM1_Init();

	SCANNER_UART2_Init(115200);
	CH340_UART1_Init(115200);
	// Send_Manual_Cmd();
	// Send_Continuous_Cmd();
	// 按键初始化
	key_Init();
	LED_State_Init();
	STATUS_LED_GREEN_ON();			// 状态灯 低电平点亮
	// Send_Single_Cmd(15);
	// Send_ReadDeviceID_Cmd();
	Send_Cmd_trig_Cmd();
	
	Delay_ms(100);
	// 清除缓冲区
	memset(rx_buffer_scanner, 0, sizeof(rx_buffer_scanner));	
	memset(Rx_scanner_temp, 0, sizeof(Rx_scanner_temp));	
	scanner_rx_index = 0;
	data_REC_scanner_Flag = false;
	
#ifdef DEBUG_PRINT	
	printf("Initialization Successful!\r\n");
#endif	
	HeartBeat_TIM_Init();
	
	
	TIM_Double_Click_Init();
	// Write_App_Version();
//	if(Erase_Flag_Sector() == FLASH_COMPLETE) {
//    printf("Erase suc!\r\n");
//    // ????????FF
//    uint32_t* p = (uint32_t*)APP_VERSION_ADDRESS;
//    if(*p == 0xFFFFFFFF) {
//        printf("Erase fail\r\n");
//    }
//} else {
//    printf("????,???:%d\r\n", FLASH_GetStatus());
//}
	// Erase_Flag_Sector();
				// printf("1");

	while(1){

		
		if(data_REC_scanner_Flag == true){
			Handle_RX_Scanner();
		}
		if(data_REC_ch340_Flag == true){
			Handle_RX_CH340();
		}
		
		
		
		if(click_event){
			click_event = 0;
			uint8_t frame[5];
			frame[0] = 0xAA;
			frame[1] = 0x01;
			frame[2] = 0x02;
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
		}
		if(double_click_event){
			double_click_event = 0;
				if(MANUAL_FLAG == true){				// 上电默认手动标志位(即命令触发标志位)为真
					TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE); 
		#ifdef 	DEBUG_PRINT
					key1 = true;
		#endif
					blink_cnt = 0;
					Send_Cmd_trig_Cmd();
					for(int i  = 0; i <= 555;i++){
						;
					}
					Send_Cmd_trig_ON_Cmd();			
						
				} // else{
			
		}
		
		
		
//		if(key1 == true && data_REC_scanner_Flag == false){
//			memset(rx_buffer_scanner, 0, sizeof(rx_buffer_scanner));	
//			memset(Rx_scanner_temp, 0, sizeof(Rx_scanner_temp));	
//			scanner_rx_index = 0;	
//		}
//		if(	scanner_rx_index != 0){
//			printf("%s",rx_buffer_scanner);
//				memset(rx_buffer_scanner, 0, sizeof(rx_buffer_scanner));	
//	memset(Rx_scanner_temp, 0, sizeof(Rx_scanner_temp));	
//	scanner_rx_index = 0;
//		}
		// printf("%s",rx_buffer_ch340);
	}
	
	
}

