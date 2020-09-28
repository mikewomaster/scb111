/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "main.h"
#include "usart.h"
#include "usb_device.h"
#include "param.h"
#include "gpio.h"
#include "nbiot.h"
#include "function.h"
#include "param_init.h"
#include "gsModbus.h"
#include "modbus_handle.h"
#include "nb_tcp.h"
#include "tim.h"
#include "nb_mqtt.h"
#include "modbus_rtu.h"
#include "modbus_rtu_sensor.h"
#include "upload_method.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/

/* USER CODE BEGIN PFP */
extern char rs485_uart_rcv_buffer[256];
extern uint16_t irqNum_idle_uart;

uint8_t usb_rcv_buff[128];
extern uint16_t rs485_uart_rcv_len;
extern uint16_t usb_rcv_flag;

extern uart_config_t uart_config_default;
extern userinfo user_info_default;

extern nbiot_config_t nbiot_config_default;
extern mqtt_config_t mqtt_config_default;
extern uint8_t b_mqtt_Config_Changed;

extern uint16_t usb_rcv_len;
uint32_t g_restore_flag;
uint32_t g_uart_config_change;
extern unsigned int gs_baudrate[10];
extern uart_config_t uart_config;

extern Param_Config_t param_config_tbl[];
extern UART_HandleTypeDef huart2;

extern mqtt_config_t mqtt_config;
extern nbiot_config_t nbiot_config;

extern int nb_tcp_rcv_flag;
extern uint8_t nb_tcp_rcv[256];
extern char g_nbiot_RxBuffer[512];
extern uint32_t g_nbiot_rx_len;
extern TIM_HandleTypeDef htim6;

extern enum usb_plug_flag usbPlugFlag;
extern int detachFlag;
extern sensorData g_sensorData;
extern uint8_t ModbusRtuSendFlag;

gsModbus modbus_slave;
gNbTCP nb_tcp;
gNbMQTT nb_mqtt;
int timer6 = 0;

#define MODEL_NAME_STR	"SCB111-485"
#define SW_VERSION_STR	"v0.3.0"
mod_name_t mod_name_default = {MODEL_NAME_STR};
software_ver sw_v_default = {SW_VERSION_STR};
uint32_t g_restore_flag;

extern int read_partition_init(partition_index, char *, uint16_t);
/* USER CODE END PFP */

/* Private user code */
/* USER CODE BEGIN 0 */
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */

int main(void)
{
  /* USER CODE BEGIN 1 */
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
	MX_GPIO_Init();
	MX_USART1_UART_Init();
	MX_USART2_UART_Init();
	MX_USB_DEVICE_Init();
	MX_TIM6_Init();
	BoardInitMcu();
	
	MX_NB_Init();
	MX_WoMaster_Init();
	MX_MQTT_Init();
	nbiotDetect();
	HAL_Delay(10);
		
	gNbTCP *pNbTcp = &nb_tcp;
  /* USER CODE END SysInit */

  /* USER CODE BEGIN 2 */
	/* USER CODE END 2 */
  /* USER CODE BEGIN WHILE */	

  while (1)
  {	
    /* USER CODE END WHILE */
    /* USER CODE BEGIN 3 */
		if (usbPlugFlag == PlugIn) {
			if (usb_rcv_flag) {
				gsModbus_Processor(&modbus_slave, usb_rcv_buff, usb_rcv_len);
				usb_rcv_flag = 0;
			}

			usbModbusProcess();
		} 
		else {
			switch(nbiot_config.ulIotMode) {
				case 0:
					if (timer6) {
						 nbiot_poweroff_on();
						 ModbusRtuUnitSndAndRcv();
						 ModbusRtuMqtt();
						 nbiot_poweroff_on();
						 timer6 = 0;
					}
					break;

				case 1:
					if (irqNum_idle_uart) {
						nbiot_poweroff_on();
						HAL_Delay(10);
						rs485Mqtt();
						HAL_Delay(10);
						nbiot_poweroff_on();

						memset(rs485_uart_rcv_buffer, 0, sizeof(rs485_uart_rcv_buffer));
						rs485_uart_rcv_len = 0;
						irqNum_idle_uart = 0;
					}
					break;

				case 2:
					tcpRcvAndSnd();
					break;
			}
		}
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Configure the main internal regulator output voltage 
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_HSI48| RCC_OSCILLATORTYPE_LSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.HSI48State = RCC_HSI48_ON;
	RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLLMUL_6;
  RCC_OscInitStruct.PLL.PLLDIV = RCC_PLLDIV_3;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1|RCC_PERIPHCLK_LPUART1
                              |RCC_PERIPHCLK_USB|RCC_PERIPHCLK_USART2|RCC_PERIPHCLK_RTC;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
	PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
  PeriphClkInit.Lpuart1ClockSelection = RCC_LPUART1CLKSOURCE_PCLK1;
	PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
  PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_HSI48;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void usbModbusProcess()
{
	if (g_uart_config_change == 1) {
		g_uart_config_change = 0;
		uartConfigure(gs_baudrate[uart_config.baudrate], uart_config.parity, uart_config.stopBits);
	}
}

static void nbiotDetect()
{
	nbiot_poweroff_on();
	HAL_Delay(10);
	nbiot_init();
	HAL_Delay(10);
	nbiot_poweroff_on();	
}

void MX_MQTT_Init(void)
{
	/*
		mqttAddrSet(&nb_mqtt);
		mqttTopicSet(&nb_mqtt);
		mqttClientIdSet(&nb_mqtt);
		mqttUsernameSet(&nb_mqtt);
		mqttPasswordSet(&nb_mqtt);
		mqttPortSet(&nb_mqtt);
		mqttIntervalSet(&nb_mqtt);
	*/

	mqttOpenSet(&nb_mqtt, mqttOpenHandle);
	mqttConnectSet(&nb_mqtt, mqttConnectHandle);
	mqttConfigureSet(&nb_mqtt, mqttConfigureHandle);
	mqttPubSet(&nb_mqtt, mqttPubHandle);
	mqttSubSet(&nb_mqtt, mqttSubHandle);
	mqttCloseSet(&nb_mqtt, mqttCloseHandle);
	mqttDisconnectSet(&nb_mqtt, mqttDisconnectHandle);
	mqttCheckSet(&nb_mqtt, mqttCheckStatusHandle);
}

void MX_WoMaster_Init(void)
{
	irqNum_idle_uart = 0;
	uint32_t modbusContentLength = sizeof(rtuModbus) / 2;

	if (read_partition_init(PARTITION_NBIOT, (char *)&nbiot_config, sizeof(nbiot_config_t)) < 0)
		memcpy(&nbiot_config, &nbiot_config_default, sizeof(nbiot_config_t));

	if (read_partition_init(PARTITION_MQTT, (char *)&mqtt_config, sizeof(mqtt_config_t)) < 0)
		memcpy(&mqtt_config, &mqtt_config_default, sizeof(mqtt_config_t));

	read_partition_modbus_rtu(PARTITION_MODBUS_RTU, (char *)(&g_sensorData.sensorConf), modbusContentLength, 0);
	read_partition_modbus_rtu(PARTITION_MODBUS_RTU, (char *)(&g_sensorData.sensorConf) + modbusContentLength, modbusContentLength, modbusContentLength);

	HAL_Delay(100);

	gsModbus_Set_ADDR(&modbus_slave, 1);
	gsModbus_Set_SendFunction(&modbus_slave, Modbus_Slave_USBPutData);
	gsModbus_Write_REG_Function(&modbus_slave, Do_W_HOLD_REG_Function);
	gsModbus_Read_REG_Function(&modbus_slave, Do_R_HOLD_REG_Function);

	tcpSetConnectHandle(&nb_tcp, TCPConnectHandle);
	tcpSetSendHandle(&nb_tcp, TCPSendHandle);
	tcpSetRcvHandle(&nb_tcp, TCPReceiveHandle);
	tcpSetCloseHandle(&nb_tcp, TCPCloseHandle);
	tcpSetDisConnectHandle(&nb_tcp, TCPDisConnectHandle);

	HAL_TIM_Base_Start_IT(&htim6);
}

void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_6|GPIO_PIN_7, GPIO_PIN_RESET);

  /*Configure GPIO pins : PA2 PA3 */
  GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF4_USART2;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PB0 PB1 PB6 PB7 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_6|GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : PA8 */
  GPIO_InitStruct.Pin = GPIO_PIN_8;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

void MX_NB_Init(void)
{
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET);
	HAL_Delay(200);
	
	// HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET);
	// HAL_Delay(1500);
	// HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET);
	
	__HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);
  __HAL_UART_ENABLE_IT(&huart1, UART_FLAG_RXNE);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	static uint16_t count = 0;

	if(htim == (&htim6))
  {
		count ++;
		if(count / (mqtt_config.ulInterval)) {
			 timer6 = 1;
			 count = 0;
		}
  }
}

void uartConfigure(uint32_t baudrate, uint32_t type, uint32_t stopbits)
{
	huart2.Instance = USART2;
  huart2.Init.BaudRate = baudrate;
  huart2.Init.WordLength = type == 0 ? UART_WORDLENGTH_8B : UART_WORDLENGTH_9B;
  huart2.Init.StopBits = stopbits;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
	__HAL_UART_ENABLE_IT(&huart2, UART_IT_IDLE);
	__HAL_UART_ENABLE_IT(&huart2, UART_FLAG_RXNE);
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
