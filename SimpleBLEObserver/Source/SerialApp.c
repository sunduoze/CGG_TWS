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
串口设备初始化，
必须在使用串口打印之前调用该函数进行uart初始化
*/
void SerialApp_Init( uint8 taskID )
{
  //调用uart初始化代码
  serialAppInitTransport();
  //记录任务函数的taskID，备用
  //sendMsgTo_TaskID = taskID;

}

/*
uart初始化代码，配置串口的波特率、流控制等
*/
void serialAppInitTransport( )
{
  halUARTCfg_t uartConfig;

  // configure UART
  uartConfig.configured           = TRUE;
  uartConfig.baudRate             = SBP_UART_BR;//波特率
  uartConfig.flowControl          = SBP_UART_FC;//流控制
  uartConfig.flowControlThreshold = SBP_UART_FC_THRESHOLD;//流控制阈值，当开启flowControl时，该设置有效
  uartConfig.rx.maxBufSize        = SBP_UART_RX_BUF_SIZE;//uart接收缓冲区大小
  uartConfig.tx.maxBufSize        = SBP_UART_TX_BUF_SIZE;//uart发送缓冲区大小
  uartConfig.idleTimeout          = SBP_UART_IDLE_TIMEOUT;
  uartConfig.intEnable            = SBP_UART_INT_ENABLE;//是否开启中断
  uartConfig.callBackFunc         = sbpSerialAppCallback;//uart接收回调函数，在该函数中读取可用uart数据

  // start UART
  // Note: Assumes no issue opening UART port.
  (void)HalUARTOpen( SBP_UART_PORT, &uartConfig );

  return;
}
  uint16 numBytes;
/*
uart接收回调函数
当我们通过pc向开发板发送数据时，会调用该函数来接收
*/
#include <string.h>
#include "simpleBLEObserver.h"
void sbpSerialAppCallback(uint8 port, uint8 event)
{
  uint8  pktBuffer[SBP_UART_RX_BUF_SIZE];
  // unused input parameter; PC-Lint error 715.
  (void)event;
  HalLcdWriteString("Data form my UART:", HAL_LCD_LINE_5 );
  //返回可读的字节
  if ( (numBytes = Hal_UART_RxBufLen(port)) > 0 ){
  	//读取全部有效的数据，这里可以一个一个读取，以解析特定的命令
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
打印一段数据
pBuffer可以包含0x00
*/
void sbpSerialAppWrite(uint8 *pBuffer, uint16 length)
{
	HalUARTWrite (SBP_UART_PORT, pBuffer, length);
}
/*
打印一个字符串
str不可以包含0x00，除非结尾
*/
void SerialPrintString(uint8 str[])
{
  HalUARTWrite (SBP_UART_PORT, str, osal_strlen((char*)str));
}
/*
打印指定的格式的数值
参数
title,前缀字符串
value,需要显示的数值
format,需要显示的进制，十进制为10,十六进制为16
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