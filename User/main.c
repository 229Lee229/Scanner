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
	
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);			// 取消PB3 PB4 PA15的JTAG模式 改为普通IO

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
	while(1){
		if(data_REC_scanner_Flag == true){
			Handle_RX_Scanner();
		}
		if(data_REC_ch340_Flag == true){
			Handle_RX_CH340();
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
