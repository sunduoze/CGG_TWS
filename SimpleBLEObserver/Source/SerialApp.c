#include "bcomdef.h"
#include "OSAL.h"
#include "OSAL_PwrMgr.h"

#include "OnBoard.h"
#include "hal_uart.h"
#include "hal_lcd.h"
#include "SerialApp.h"

uint16 beacon_rssi_g = 0;
uint8 ble_mac_addr_g[MAC_ADDR_SIZE + 2] = "0x60C0BF29DA01";

//static uint8 sendMsgTo_TaskID;

/*
�����豸��ʼ����
������ʹ�ô��ڴ�ӡ֮ǰ���øú�������uart��ʼ��
*/
void SerialApp_Init( uint8 taskID )
{
  //����uart��ʼ������
  serialAppInitTransport();
  //��¼��������taskID������
  //sendMsgTo_TaskID = taskID;

}

/*
uart��ʼ�����룬���ô��ڵĲ����ʡ������Ƶ�
*/
void serialAppInitTransport( )
{
  halUARTCfg_t uartConfig;

  // configure UART
  uartConfig.configured           = TRUE;
  uartConfig.baudRate             = SBP_UART_BR;//������
  uartConfig.flowControl          = SBP_UART_FC;//������
  uartConfig.flowControlThreshold = SBP_UART_FC_THRESHOLD;//��������ֵ��������flowControlʱ����������Ч
  uartConfig.rx.maxBufSize        = SBP_UART_RX_BUF_SIZE;//uart���ջ�������С
  uartConfig.tx.maxBufSize        = SBP_UART_TX_BUF_SIZE;//uart���ͻ�������С
  uartConfig.idleTimeout          = SBP_UART_IDLE_TIMEOUT;
  uartConfig.intEnable            = SBP_UART_INT_ENABLE;//�Ƿ����ж�
  uartConfig.callBackFunc         = sbpSerialAppCallback;//uart���ջص��������ڸú����ж�ȡ����uart����

  // start UART
  // Note: Assumes no issue opening UART port.
  (void)HalUARTOpen( SBP_UART_PORT, &uartConfig );

  return;
}
  uint16 numBytes;
/*
uart���ջص�����
������ͨ��pc�򿪷��巢������ʱ������øú���������
*/
#include <string.h>
#include "simpleBLEObserver.h"
void sbpSerialAppCallback(uint8 port, uint8 event)
{
  uint8  pktBuffer[SBP_UART_RX_BUF_SIZE];
  // unused input parameter; PC-Lint error 715.
  (void)event;
  HalLcdWriteString("Data form my UART:", HAL_LCD_LINE_5 );
  //���ؿɶ����ֽ�
  if ( (numBytes = Hal_UART_RxBufLen(port)) > 0 ){
  	//��ȡȫ����Ч�����ݣ��������һ��һ����ȡ���Խ����ض�������
	(void)HalUARTRead (port, pktBuffer, numBytes);
        if(strncmp((const char *)"scan_beacon\r\n", (const char *)pktBuffer, numBytes) == 0)
        {
          scan_task();
        }
        else if(strncmp((const char *)"get_rssi\r\n", (const char *)pktBuffer, numBytes) == 0)
        {
          if(beacon_rssi_g > 0 && beacon_rssi_g < 100)
          {
            SerialPrintValue("rssi", beacon_rssi_g, 10);
            HalUARTWrite (SBP_UART_PORT, "\r\n", 2);
            HalLcdWriteStringValue("rssi\r\n", beacon_rssi_g, 10 ,HAL_LCD_LINE_7);      
          }
          else
          {
            LCD_WRITE_STRING( "              ", HAL_LCD_LINE_7 ); 
          }
        }
        else if(strncmp((const char *)"set_addr:", (const char *)pktBuffer, 9) == 0)
        {
          osal_memset(ble_mac_addr_g + 2, 0, MAC_ADDR_SIZE);
          osal_memcpy( ble_mac_addr_g + 2, pktBuffer + 9 , MAC_ADDR_SIZE);
          HalUARTWrite (SBP_UART_PORT, ble_mac_addr_g, MAC_ADDR_SIZE + 2);
          HalUARTWrite (SBP_UART_PORT, "\r\n", 2);
          LCD_WRITE_STRING(ble_mac_addr_g, HAL_LCD_LINE_7 );
            
        }
	HalLcdWriteString((char*)pktBuffer, HAL_LCD_LINE_6 );
  }
  
}


/*
��ӡһ������
pBuffer���԰���0x00
*/
void sbpSerialAppWrite(uint8 *pBuffer, uint16 length)
{
	HalUARTWrite (SBP_UART_PORT, pBuffer, length);
}
/*
��ӡһ���ַ���
str�����԰���0x00�����ǽ�β
*/
void SerialPrintString(uint8 str[])
{
  HalUARTWrite (SBP_UART_PORT, str, osal_strlen((char*)str));
}
/*
��ӡָ���ĸ�ʽ����ֵ
����
title,ǰ׺�ַ���
value,��Ҫ��ʾ����ֵ
format,��Ҫ��ʾ�Ľ��ƣ�ʮ����Ϊ10,ʮ������Ϊ16
*/
void SerialPrintValue(char *title, uint16 value, uint8 format)
{
  uint8 tmpLen;
  uint8 buf[256];
  uint32 err;

  tmpLen = (uint8)osal_strlen( (char*)title );
  osal_memcpy( buf, title, tmpLen );
  buf[tmpLen] = ' ';
  err = (uint32)(value);
  _ltoa( err, &buf[tmpLen+1], format );
  SerialPrintString(buf);		
}

void ext_print_uart_str(uint8 str[])
{
  HalUARTWrite (SBP_UART_PORT, str, osal_strlen((char*)str));
}