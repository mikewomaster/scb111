#if 0

	// power on
	// HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET);
	// HAL_Delay(1000);
		/*	
	#define 	SRAM_ADDR 0
	define 	READ_NUM  10

	SRAM_Init();
	
	// SRAM_Reset();
	// SRAM_Set_Burst();
	
	uint8_t Sram_ID_Buf[8]={0};
	uint8_t Sram_buf[256]={0};
	uint8_t Sram_buf2[256]={0};
	uint16_t di=0, di2=0;
	
	SRAM_Read_ID(0, Sram_ID_Buf, 8);
	DebugPrintf("SRAM ID: ");
	for(di=0;di<8;di++)
	{
		DebugPrintf("%02x ", Sram_ID_Buf[di]);
		HAL_Delay(2);
	}
	DebugPrintf("\r\n");
	

	Lable_B:
	SRAM_Read(SRAM_ADDR, Sram_buf, READ_NUM);
	DebugPrintf("Read from SRAM: ");
	for(di=0;di<READ_NUM;di++)
	{
		DebugPrintf("%02x ", Sram_buf[di]);
		HAL_Delay(2);
	}
	DebugPrintf("\r\n");
	
	srand(HAL_GetTick());
	di2 = rand();
	for(di=0;di<READ_NUM;di++)
	{
		Sram_buf2[di] = di2++;
	}
	DebugPrintf("Write SRAM: ");
	for(di=0;di<READ_NUM;di++)
	{
		DebugPrintf("%02x ", Sram_buf2[di]);
		HAL_Delay(2);
	}
	DebugPrintf("\r\n");	

	SRAM_Write(SRAM_ADDR, Sram_buf2, READ_NUM);
	HAL_Delay(1000);
	goto Lable_B;
	
	while(1);	
*/
	
	// relay: Di-Do
	// HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
	// HAL_Delay(3000);
	// HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
  
	/* USER CODE END 2 */
  /* Infinite loop */

	// MX_DMA_Init();
 	// MX_LPUART1_UART_Init();
	
	// HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
	// HAL_Delay(3000);
	// HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
			
	// MQTT
  // DebugPrintf("nbiot connect server");
	// nbiot_updateSettings();
	// DebugPrintf("connect server done\n");
	// 
	// DebugPrintf("nbiot update rtc");
	// nbiot_updateRTC();
	// DebugPrintf("rtc update done\n");
	// 
	// DebugPrintf("mqtt connect");
	// nbiot_updateMQTT();
	// DebugPrintf("mqtt connect done\n");
	// 
	// 		
	// DebugPrintf("mqtt one shot");
	// if( error == 0 )
	// {		
	// 	nbiot_sendMQTT();				
	// }
	// else
	// {
	// 	nbiot_sendMQTT2();		
	// }
	// DebugPrintf("mqtt shot done\n");	
	// 
	// nbiot_disconnect();
	// nbiot_sleep();
	
			// UART
		/*
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);		
			if(HAL_UART_Receive(&huart2, (uint8_t *)aRxBuffer, 10, 10000) != HAL_OK)
				Error_Handler();
			
			HAL_Delay(50);
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
			HAL_UART_Transmit(&huart2, aRxBuffer, 10, 0xffff);
		*/
		
			DebugPrintf("\r\n");
			DebugPrintf((char *) usb_rcv_buff);
			memset(usb_rcv_buff, 0, sizeof(usb_rcv_buff));
			usb_rcv_flag = 0;
			
					uart_config_t uart_config_default = {0, 8, 1, 0, 1};
		if( 0 != write_partition(PARTITION_UART, (char *)&uart_config_default, 10)) {
			DebugPrintf("write wrong");
			return -1;
		} else {
			DebugPrintf("write success\r\n");
		}

		uart_config_t buff_temp = {0, 0, 0, 0, 0};
		DebugPrintf("%d, %d, %d, %d, %d\r\n", buff_temp.baudrate, buff_temp.wordLength, buff_temp.stopBits, buff_temp.parity, buff_temp.modbus_addr);
		read_partition(PARTITION_UART, (char *)&buff_temp, 10);
		DebugPrintf("%d, %d, %d, %d, %d\r\n", buff_temp.baudrate, buff_temp.wordLength, buff_temp.stopBits, buff_temp.parity, buff_temp.modbus_addr);
		HAL_Delay(500000);
		
				lora_config_t lora_cfg = {1, 2, 3, 4, 5, 6};
		write_partition(PARTITION_LOAR, (char *)&lora_cfg, sizeof(lora_cfg));
		lora_config_t lora_read_buf;
		read_partition(PARTITION_LOAR, (char *)&lora_read_buf, sizeof(lora_cfg));
		DebugPrintf("%d-%d-%d-%d-%d\n", lora_read_buf.freq_band, lora_read_buf.sf, lora_read_buf.bandwidth, lora_read_buf.cr, lora_read_buf.lro);
		HAL_Delay(10000);
		
		if( 0 != write_partition(PARTITION_NBIOT, (char *)&nbiot_config_default, sizeof(nbiot_config_t)) ) {
			DebugPrintf("write wrong");
		} else {
			DebugPrintf("write success\r\n");
		}
		DebugPrintf("%s\r\n", nbiot_config_default.szUserName);
		HAL_Delay(10000);
			
				/*
			nbiot_config_t buff_temp2;
			read_partition(PARTITION_NBIOT, (char *)&buff_temp2, sizeof(nbiot_config_t));
			DebugPrintf("%s-%s\r\n", buff_temp2.szUserName, buff_temp2.szUserPassword);
			
			uart_config_t uart_config_default = {0, 8, 1, 0, 1};
			if( 0 != write_partition(PARTITION_UART, (char *)&uart_config_default, 10)) {
				DebugPrintf("write wrong");
				return -1;
			} else {
				DebugPrintf("write success\r\n");
			}

			uart_config_t buff_temp = {0, 0, 0, 0, 0};
			DebugPrintf("%d, %d, %d, %d, %d\r\n", buff_temp.baudrate, buff_temp.wordLength, buff_temp.stopBits, buff_temp.parity, buff_temp.modbus_addr);
			read_partition(PARTITION_UART, (char *)&buff_temp, 10);
			DebugPrintf("%d, %d, %d, %d, %d\r\n", buff_temp.baudrate, buff_temp.wordLength, buff_temp.stopBits, buff_temp.parity, buff_temp.modbus_addr);
			HAL_Delay(5000);
		*/
		
			// 
	// // enable timer
	// mqtt_count_down_timer = (mqtt_config.ulInterval);
	// mqtt_count_down_timer_on = 1;
	// mqtt_count_down_timer_timeout = 0;
	// return ret;
	
	
	HAL_Delay(1000);
	DebugPrintf("start tcp send ...\r\n");
	memset(cmd,'\0',sizeof(cmd));
	sprintf(cmd,"AT+CASEND=0,5");
	cmdRet = Send_AT_Command(cmd, ">", 2000, 2000);
	char szMsg[5] = {'a', 'b', 'c', 'd', '\0'};
	cmdRet = Send_AT_Command(szMsg,"OK", 5000, 5000);
	
	HAL_Delay(10000);
	DebugPrintf("start tcp rcv ...\r\n");
	memset(cmd,'\0',sizeof(cmd));
	sprintf(cmd,"AT+CARECV=0,100");
	cmdRet = Send_AT_Command(cmd,"OK",2000,2000);
	DebugPrintf("g_nbiot_RxBuffer=%s, :::%d\r\n", g_nbiot_RxBuffer, strlen(g_nbiot_RxBuffer));
		
		
				
		char *p = strstr(g_nbiot_RxBuffer, "+CARECV: ");
		if (p)
			DebugPrintf("find:%s\r\n", p);
		else
			DebugPrintf("not find");
		
		if (strstr(g_nbiot_RxBuffer, "+CARECV: ")) {
			char *p = g_nbiot_RxBuffer + 9;
			char *q = NULL; 
			if ((q = strstr(p, ",")) != NULL) {
					char buf[8];
					strncpy(buf, p, q-p);
				  DebugPrintf(buf);
			}
		}

		if (strstr(cmd, "CARECV")) {
			char *p = strstr(g_nbiot_RxBuffer, "+CARECV: ");
			char *q = NULL;
			
			if (p) {
				q = strstr(p, ",");
				if (q) {
					  char *end = strstr(q, "\r\n");
					  char buf[20] = {0};
						memcpy(buf, q+1, end-q-1);
						DebugPrintf("buffer:%s\r\n", buf);
				}
			}
		}

	// FIX ME: remove to reset function
	write_partition(PARTITION_UART, (char *)&uart_config_default, param_config_tbl[0].num);
	HAL_Delay(100);
	// write_partition(PARTITION_USER_INFO, (char *)&user_info_default, 	 param_config_tbl[8].num);
  write_partition(PARTITION_NBIOT, (char *)&nbiot_config_default, sizeof(nbiot_config_t));
	HAL_Delay(100);
	write_partition(PARTITION_MQTT, (char *)&mqtt_config_default, sizeof(mqtt_config_t));
	HAL_Delay(100);
	
			if( 0 != write_partition(PARTITION_UART, (char *)&uart_config_default, param_config_tbl[0].num))
			return -1;
		
		DebugPrintf("%d-%d\r\n",uart_config_default.baudrate, uart_config_default.parity);
				
		HAL_Delay(1000);
		
		if( 0!= write_partition(PARTITION_USER_INFO, (char *)&user_info_default, 	 sizeof(userinfo)))
			return -1;
		
		DebugPrintf("%s-%s\r\n",user_info_default.password, user_info_default.username);
		
		HAL_Delay(10000);
		
				#if 0
		nbiot_config_t buff_temp;
		
		strcpy(nbiot_config_default.szUserName, "adi\r\n");
		write_partition(PARTITION_NBIOT, (char *)&nbiot_config_default, sizeof(nbiot_config_t));


		read_partition(PARTITION_UART, (char *)&buff_temp, sizeof(nbiot_config_t));
		DebugPrintf(nbiot_config_default.szUserName);
		
		strcpy(nbiot_config_default.szUserName, "das\r\n");
		write_partition(PARTITION_NBIOT, (char *)&nbiot_config_default, sizeof(nbiot_config_t));
		
		read_partition(PARTITION_UART, (char *)&buff_temp, sizeof(nbiot_config_t));
		DebugPrintf(nbiot_config_default.szUserName);
		
		HAL_Delay(5000);
		#endif
		
		
		#if 0
		Send_sensor_data(0);
		gNbTCP *pNbTcp = &nb_tcp;
		pNbTcp->tcpConnect();

		for (;;) {
			HAL_Delay(100);
			char buff[10] = {'a', 'b', 'c', 'e', 'd', 'h'};
			pNbTcp->tcpSend((unsigned char*)buff);

			HAL_Delay(8000);
			pNbTcp->tcpReceive((unsigned char*) buff);

			HAL_Delay(10000);
		}

		// handle data from tcp server to serial 

		HAL_Delay(10000);
		#endif
		
		// HAL_Delay(10000);
		
		// write_partition(4, "SCB111-NB-485", param_config_tbl[4].num);
		
		
		int Send_AT_Command_rcv(char *cmd, char *result, uint32_t wait_time, uint32_t wait_time2)
{
	int ret = 0;
	uint8_t cmdRet = 0;
	uint8_t at_cmd[CMD_LENGTH]={'\0'};
	uint32_t at_cmd_length = 0;
	uint32_t u32StartTime = 0;
	uint8_t SendCmd = 0;
	int index1 = 0,index2 = 0;
	char *pch1 = NULL,*pch2 = NULL;
	int i = 0;
	int length = 0;
	
	if(cmd == NULL || result == NULL)
		return 0;

	if(strlen(cmd) == 0 || strlen(cmd) > CMD_LENGTH)
		return 0;
		
	irqNum_idle=0;
	at_cmd_length = strlen(cmd);
	memcpy(at_cmd, cmd, strlen(cmd));
	if(memcmp(at_cmd,"{",1) != 0)
		{
		if(cmd[(at_cmd_length-1)] != '\r')
			{
			if(at_cmd_length < CMD_LENGTH)
				{
				at_cmd_length = at_cmd_length+1;
				at_cmd[at_cmd_length-1] = '\r';
				}
			else
				return -2;
			}
		}

	CLR_NBIOT_RxBuf();
	ret = 0;
	do
	  {
	  if(strlen(result) > 0 && Find(result))
	  	{
	  	ret = 1;
	  	}
	  else if(strlen(result) == 0 && irqNum_idle > 1)
	  	{
	  	ret = 1;
	  	}
	  else if(Find("ERROR"))
	  	{
	  	ret = -1;
	  	}
	  else
	  	{
	  	while(__HAL_UART_GET_FLAG(&huart1, UART_FLAG_TC) == RESET);
	  	if(SendCmd == 0 || ((HAL_GetTick()-u32StartTime)>wait_time2) || !Find(result))
	  		{
				u32StartTime = HAL_GetTick();	
	  		cmdRet = HAL_UART_Transmit(&huart1, at_cmd, at_cmd_length, wait_time);

	  		if(cmdRet != HAL_OK)
	  			ret = -1;
	  		else
	  			SendCmd = 1;
	  		}
	  	}
			DelayMs(20);
	  }while(ret == 0);

		DebugPrintf("g_nbiot_RxBuffer=%s \r\n",g_nbiot_RxBuffer);

	if(ret == 1 && strlen(result) > 0)
		{
		pch1 = strstr(g_nbiot_RxBuffer,cmd);		
		if(pch1 != NULL)
			{
			index1 = (pch1 - g_nbiot_RxBuffer) + strlen(cmd);
			do
			  {
			  if(index1 >= BUFFER_SIZE)
			  	break;
			  	
			  if(g_nbiot_RxBuffer[index1] == '\r' || g_nbiot_RxBuffer[index1] == '\n')
			  	index1++;
			  else
			  	break;
			  }while(1);
				
			pch2 = strstr(&g_nbiot_RxBuffer[index1], result);
			if(pch2 != NULL)
				{
				index2 = pch2 - &g_nbiot_RxBuffer[index1];
				length = index2;
				for(i = (index1+index2-1); i > 0; i--)
					{
					if(g_nbiot_RxBuffer[i] == '\r' || g_nbiot_RxBuffer[i] == '\n')
						length--;
					else
						break;
					}

				if(length > 0)
					{
					memcpy(g_nbiot_RxBuffer, &g_nbiot_RxBuffer[index1], length);
					g_nbiot_rx_len = length;
					memset(&g_nbiot_RxBuffer[g_nbiot_rx_len], '\0', (BUFFER_SIZE-g_nbiot_rx_len));
					}
				}
			}
		}
	
	return ret;
}

int Send_AT_Command2(char *cmd, char *result, char *result2, uint32_t wait_time, uint32_t wait_time2)
{
	int ret = 0;
	uint8_t cmdRet = 0;
	uint8_t at_cmd[CMD_LENGTH]={'\0'};
	uint32_t at_cmd_length = 0;
	uint32_t u32StartTime = 0;
	uint8_t SendCmd = 0;
	int index1 = 0,index2 = 0;
	char *pch1 = NULL,*pch2 = NULL;
	int i = 0;
	int length = 0;
	
	if(cmd == NULL || result == NULL)
		return 0;

	if(strlen(cmd) == 0 || strlen(cmd) > CMD_LENGTH)
		return 0;
		
	irqNum_idle=0;
	at_cmd_length = strlen(cmd);
	memcpy(at_cmd, cmd, strlen(cmd));
	if(memcmp(at_cmd,"{",1) != 0)
		{
		if(cmd[(at_cmd_length-1)] != '\r')
			{
			if(at_cmd_length < CMD_LENGTH)
				{
				at_cmd_length = at_cmd_length+1;
				at_cmd[at_cmd_length-1] = '\r';
				}
			else
				return -2;
			}
		}

	CLR_NBIOT_RxBuf();
	ret = 0;
	do
	  {
	  if(strlen(result) > 0 && Find(result))
	  	{
	  	ret = 1;
	  	}
	  else if(strlen(result) == 0 && irqNum_idle > 1)
	  	{
	  	ret = 1;
	  	}
	  else if(Find("ERROR"))
	  	{
	  	ret = -1;
	  	}
	  else
	  	{
	  	while(__HAL_UART_GET_FLAG(&huart1, UART_FLAG_TC) == RESET);
	  	if(SendCmd == 0 || ((HAL_GetTick()-u32StartTime)>wait_time2))
	  		{
				u32StartTime = HAL_GetTick();	
	  		cmdRet = HAL_UART_Transmit(&huart1, at_cmd, at_cmd_length, wait_time);

	  		if(cmdRet != HAL_OK)
	  			ret = -1;
	  		else
	  			SendCmd = 1;
	  		}
	  	}
			DelayMs(20);
	  }while(ret == 0);

		DebugPrintf("g_nbiot_RxBuffer=%s \r\n",g_nbiot_RxBuffer);

	if(ret == 1 && strlen(result) > 0)
		{
		pch1 = strstr(g_nbiot_RxBuffer,cmd);		
		if(pch1 != NULL)
			{
			index1 = (pch1 - g_nbiot_RxBuffer) + strlen(cmd);
			do
			  {
			  if(index1 >= BUFFER_SIZE)
			  	break;
			  	
			  if(g_nbiot_RxBuffer[index1] == '\r' || g_nbiot_RxBuffer[index1] == '\n')
			  	index1++;
			  else
			  	break;
			  }while(1);
				
			pch2 = strstr(&g_nbiot_RxBuffer[index1], result);
			if(pch2 != NULL)
				{
				index2 = pch2 - &g_nbiot_RxBuffer[index1];
				length = index2;
				for(i = (index1+index2-1); i > 0; i--)
					{
					if(g_nbiot_RxBuffer[i] == '\r' || g_nbiot_RxBuffer[i] == '\n')
						length--;
					else
						break;
					}

				if(length > 0)
					{
					memcpy(g_nbiot_RxBuffer, &g_nbiot_RxBuffer[index1], length);
					g_nbiot_rx_len = length;
					memset(&g_nbiot_RxBuffer[g_nbiot_rx_len], '\0', (BUFFER_SIZE-g_nbiot_rx_len));
					}
				}
			}
		}
	
	return ret;
}

	// enter psm
	// AT+CZPM=0
	// memset(cmd,'\0',sizeof(cmd));
	// strcpy(cmd,"AT+CZPM=0");
	// cmdRet = Send_AT_Command(cmd,"OK",CMD_TIMEOUT,2000);
	// if(cmdRet == 1 && g_nbiot_rx_len > 0)
	// {
	// 
	// 
	// }
	
			// if (strstr(g_nbiot_RxBuffer, "CARECV"))
			// nb_tcp_rcv_flag = 1;
	
		if (strstr(g_nbiot_RxBuffer, "CARECV")) {
			memset(nb_tcp_rcv, 0, sizeof(nb_tcp_rcv));
			nb_tcp_rcv_flag = 1;
			// FIXME: buffer
			
			char *p = NULL, *q = NULL;
			if ((p = strstr(g_nbiot_RxBuffer, ":")) != NULL)
				if ((q = strstr(p, "\r\n")) !=  NULL)
					DebugPrintf("%s-%s\r\n", p, q);
		}
		
					if (detachFlag == 1) {
					DebugPrintf("sleep for bull shit\r\n");
					HAL_Delay(3000);
					MX_NB_Init();
					HAL_Delay(2000);
					nbiot_init();
					pNbTcp->tcpConnect();
					detachFlag = 0;
				}
				
				if (nb_time_flag == 1) {
				DebugPrintf("\r\n\r\nLinkinPark watchDog...\r\n\r\n");
				// nbiot_init();
				// pNbTcp->tcpConnect();
				// HAL_Delay(2000);
				// pNbTcp->tcpReceive();
				// HAL_Delay(10);
				// pNbTcp->tcpClose();
				// nb_time_flag = 0;
			}
			
			void Send_sensor_data(uint8_t error)
{
	 DebugPrintf("nbiot init\r\n");
	 nbiot_init();
	 DebugPrintf("\r\ninit done\r\n");
}

			HAL_Delay(10000);
			rtuUnit tmp;
			read_partition(9, (char *) &tmp, sizeof(tmp));
			DebugPrintf("%s-%d-%d-%d\r\n", tmp.name, tmp.addr, tmp.reg, tmp.num);
			
				/*
		set_value_str(p, 8, tmp);
		set_value_uint16(p+8, 2, tmp+8);
		set_value_uint16(p+10, 2, tmp+10);
		set_value_uint16(p+12, 2, tmp+12);
	*/
	
	/*		
		for (i = 0; i < num; i++) {
			for (j = 0; j < 14; j++) {
				if (j == 0) {
					ret = get_value_str(tmp+i*14, 8, p+i*14);
				} else if (j == 8 || j == 10 || j == 12) {
					memcpy(p+i*14+j, tmp+i*14+j, 2);
					// ret = get_value_uint32(tmp+i*14+j, 2, );
				}
			}
		}
*/

			for (int i = 0; i <= 6; i++)
				DebugPrintf(" %d,", rs485_uart_rcv_buffer[i]);
			DebugPrintf("\r\n");
			
						// UnitTest:
			/*
			HAL_Delay(10000);
			test_comp();
			test_snd();
			HAL_Delay(10);
			test_rcv();
			*/
			
			
			#ifdef __UNIT_TEST__
uint8_t sensorRead[8];

void test_comp()
{
	memset(sensorRead, 0, sizeof(sensorRead));

	sensorRead[0] = 0x01;
	sensorRead[1] = 0x03;
	sensorRead[2] = 0x00;
	sensorRead[3] = 0x05;
	sensorRead[4] = 0x00;
	sensorRead[5] = 0x01;

	unsigned short res = crc_Compute(sensorRead, 6);
	sensorRead[6] = res & 0x00ff;
	sensorRead[7] = res >> 8;
}

void test_snd()
{
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
	HAL_UART_Transmit(&huart2, sensorRead, sizeof(sensorRead), 10000);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
}

void test_rcv()
{
	if (irqNum_idle_uart)
	{
			rs485_uart_rcv_len = 0;
			irqNum_idle_uart = 0;
			for (int i = 0; i <= 6; i++)
				DebugPrintf(" %d,", rs485_uart_rcv_buffer[i]);
			DebugPrintf("\r\n");

			memset(rs485_uart_rcv_buffer, 0, sizeof(rs485_uart_rcv_buffer));
	}
}
#endif

	for (int i = 0; i < 20; i++) {
		if (g_sensorData.sensorConf.rtuModbusTable[i].name[1] == '\0')
			continue;
		
		short modbusNameLen = strlen(g_sensorData.sensorConf.rtuModbusTable[i].name) + 2;
		char modbusBuf[16] = {0};
		
		get_value_str(g_sensorData.sensorConf.rtuModbusTable[i].name, modbusNameLen, modbusBuf);
		DebugPrintf("%s-%d-%d\r\n", modbusBuf, g_sensorData.sensorConf.rtuModbusTable[i].addr, g_sensorData.sensorConf.rtuModbusTable[i].reg);
	}
			
		for (int i = 0; i < 20; i++) {
			if (g_sensorData.sensorConf.rtuModbusTable[i].name[1] == '\0')
				continue;
			
			short modbusNameLen = strlen(g_sensorData.sensorConf.rtuModbusTable[i].name) + 2;
			char modbusBuf[16] = {0};
			
			get_value_str(g_sensorData.sensorConf.rtuModbusTable[i].name, modbusNameLen, modbusBuf);
			DebugPrintf("%s-%d-%d\r\n", modbusBuf, g_sensorData.sensorConf.rtuModbusTable[i].addr, g_sensorData.sensorConf.rtuModbusTable[i].reg);
		}
	}
	
	pNbMqtt->mqttOpen();

	mqttConfig type;
	for (type = URL; type < ClientID; type++) {
		pNbMqtt->mqttConfigure(type);
		HAL_Delay(10);
	}
	pNbMqtt->mqttConnect();

	// FIX ME: Set MQTT Upload String
	char str[64] = {0};
	sprintf(str, "{modelname: %s}", MODEL_NAME_STR);
	
	pNbMqtt->mqttPub(str, strlen(str));
	pNbMqtt->mqttDisconnect();
	pNbMqtt->mqttClose();

	HAL_Delay(15000);
	
		if (usbPlugFlag == PlugOut)
	{
		// nbiot_init();
		// pNbTcp->tcpConnect();
	}
	
	
			char *p_mqtt_t = (char*) &mqtt_config;
		
			DebugPrintf("%s,%s,%s,%s,%s", p_mqtt_t+0, p_mqtt_t+34, p_mqtt_t+98, p_mqtt_t+134, p_mqtt_t+150);
			HAL_Delay(10000);
#endif
