#ifndef __SCANNER_H
#define __SCANNER_H


void Send_Manual_Cmd(void);
void Send_Continuous_Cmd(void);
void Send_ReadDeviceID_Cmd(void);

void Handle_RX_Scanner(void);   		// ������յ���scanner����,�������ݴ���usart1
void Handle_RX_CH340(void);				// ������յ���CH340����,����

void Send_Single_Cmd(int second);

void Send_Cmd_trig_Cmd(void);
void Send_Cmd_trig_ON_Cmd(void);
#endif
