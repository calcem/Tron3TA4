#include "main.h"

//memory location to write to in the device
__IO uint16_t memLocation = 0x000A; //pick any location within range
	void ExtPBNum2(void);
	void PB_Config(void);
uint8_t Tx1_Buffer = 'B';
uint8_t Rx1_Buffer;
uint16_t NumDataRead = 1;
uint8_t toBeSaved = '7';
int counter = 0;
uint16_t line;
int x = 0, y = 0;
__IO uint8_t UBPressed = 0;

__IO uint8_t EB1Pressed = 0;
__IO uint8_t EB2Pressed = 0;
		char test1[15];
		char time[15];
		char saved[15];
		int hours;
		int minutes;
		int seconds;
		//0 - nothing
		//1 - hours
		//2 - minutes
		//3 - seconds
		int state = 0;
		
		RTC_InitTypeDef RTC_InitStructure;
		RTC_TimeTypeDef RTC_TimeStructure;
		RTC_DateTypeDef RTC_DateStructure;
int main(void){
	
	//configure push-button interrupts
	PB_Config();
	
	 /* LCD initiatization */
  LCD_Init();
  
  /* LCD Layer initiatization */
  LCD_LayerInit();
    
  /* Enable the LTDC */
  LTDC_Cmd(ENABLE);
  
  /* Set LCD foreground layer */
  LCD_SetLayer(LCD_FOREGROUND_LAYER);

	
	
	//======You need to develop the following functions======
	//Note: these are just placeholders; function definitions are at bottom of this file
	//configure real-time clock
	RTC_Config();
	
	//configure external push-buttons and interrupts
	ExtPB_Config();
	ExtPBNum2();
	
	
	//main program
	
	LCD_Clear(LCD_COLOR_WHITE);
		
	//line=0;
	//Display a string in one line, on the first line (line=0)
	//LCD_DisplayStringLine(LINE(line),  (uint8_t *) "Init EEPROM...");
	//line++;
	
	//i2c_init(); //initialize the i2c chip
	sEE_Init();  

	
	//LCD_DisplayStringLine(LINE(line),  (uint8_t *) "done..."); 
	//line++;
	
	//LCD_DisplayStringLine(LINE(line),  (uint8_t *) "Writing...");
	//line++;
	
	
	/* First write in the memory followed by a read of the written data --------*/
  /* Write on I2C EEPROM from memLocation */
  //sEE_WriteBuffer(&Tx1_Buffer, memLocation,1); 

  /* Wait for EEPROM standby state */
  //sEE_WaitEepromStandbyState();  
 
  
	//LCD_DisplayStringLine(LINE(line),  (uint8_t *) "Reading...");
  /* Read from I2C EEPROM from memLocation */
  //sEE_ReadBuffer(&Rx1_Buffer, memLocation, (uint16_t *)(&NumDataRead)); 
	//line++;
	
	//LCD_DisplayStringLine(LINE(line),  (uint8_t *) "Comparing...");  
	//line++;
	
	
	//if(Tx1_Buffer== Rx1_Buffer){
		//LCD_DisplayStringLine(LINE(line),  (uint8_t *) "Success!");  
	//}else{
		//LCD_DisplayStringLine(LINE(line),  (uint8_t *) "Mismatch!"); 
	//}
	
	//main loop
	while(1){
		RTC_GetTime(RTC_Format_BIN,&RTC_TimeStructure);
		hours = RTC_TimeStructure.RTC_Hours;
		minutes = RTC_TimeStructure.RTC_Minutes;
		seconds = RTC_TimeStructure.RTC_Seconds;

		sprintf(time,"%0.2d:%0.2d:%0.2d",hours,minutes,seconds);
		LCD_DisplayStringLine(LINE(6),  (uint8_t *) time); 
		
		if(UBPressed == 1){
			
			toBeSaved = time[7];
			sEE_WriteBuffer(&toBeSaved, memLocation+1,1); 
			sEE_WaitEepromStandbyState();  
			
			sEE_ReadBuffer(&Rx1_Buffer, memLocation+1, (uint16_t *)(&NumDataRead)); 
			saved[0] = Rx1_Buffer;

			
			
			LCD_DisplayStringLine(LINE(7),  (uint8_t *) saved);
			
			UBPressed = 0;
			PB_Config();
			
		}
		if(EB1Pressed == 1 && state == 0){
			state = 1;
			EB1Pressed = 0;
			ExtPB_Config();
		}
		if(EB1Pressed == 1 && state == 1){
			state = 2;
			EB1Pressed = 0;
			ExtPB_Config();
			
		}
		if(EB1Pressed == 1 && state == 2){
			state = 3;
			EB1Pressed = 0;
			ExtPB_Config();
			
		}
		if(EB1Pressed == 1 && state == 3){
			state = 0;
			EB1Pressed = 0;
			ExtPB_Config();
			
		}
		if(EB2Pressed == 1 && state == 1){
			
			RTC_TimeStructure.RTC_Hours = hours + 1;
			RTC_SetTime(RTC_Format_BCD, &RTC_TimeStructure);
			EB2Pressed = 0;
			ExtPBNum2();
		}
		if(EB2Pressed == 1 && state == 2){
			
			RTC_TimeStructure.RTC_Minutes = minutes + 1;
			RTC_SetTime(RTC_Format_BCD, &RTC_TimeStructure);
			EB2Pressed = 0;
			ExtPBNum2();
		}
		if(EB2Pressed == 1 && state == 3){
			
			RTC_TimeStructure.RTC_Seconds = seconds + 1;
			RTC_SetTime(RTC_Format_BCD, &RTC_TimeStructure);
			EB2Pressed = 0;
			ExtPBNum2();
		}
		
	}
}



void PB_Config(void)
{
/* Initialize User_Button on STM32F4-Discovery
   * Normally one would need to initialize the EXTI interrupt
   * to handle the 'User' button, however the function already
   * does this.
   */
  STM_EVAL_PBInit(BUTTON_USER, BUTTON_MODE_EXTI);
}


/**
 * Use this function to configure the GPIO to handle input from
 * external pushbuttons and configure them so that you will handle
 * them through external interrupts.
 */
void ExtPB_Config(void){
		//initialize the structures we will need
	GPIO_InitTypeDef GPIO_InitStruct;
	EXTI_InitTypeDef EXTI_InitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
	
	//initialize the clocks
	//not sure if they are already initialized somewhere
	//better safe than sorry
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	
	//set the pin mode to in - as in its taking input
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
	//using pin PC4 -- so Pin_4
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
	
	//set to GPIOC because PC4 -- next 2 lines
	GPIO_Init(GPIOC, &GPIO_InitStruct);
	//also set PinSource to 4
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource4);
	
	//once again line 4
	EXTI_InitStruct.EXTI_Line = EXTI_Line4;
	EXTI_InitStruct.EXTI_LineCmd = ENABLE;
	EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
	//could get away with EXTI_Trigger_Rising or EXTI_Trigger_Falling
	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
	EXTI_Init(&EXTI_InitStruct);
	
	
	NVIC_InitStruct.NVIC_IRQChannel = EXTI4_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0x00;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0x00;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);
	
	EXTI_ClearITPendingBit(EXTI_Line4);
}


void ExtPBNum2(void){
		//initialize the structures we will need
	GPIO_InitTypeDef GPIO_InitStruct;
	EXTI_InitTypeDef EXTI_InitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
	
	//initialize the clocks
	//not sure if they are already initialized somewhere
	//better safe than sorry
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	
	//set the pin mode to in - as in its taking input
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
	//using pin PC4 -- so Pin_4
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
	
	//set to GPIOC because PC4 -- next 2 lines
	GPIO_Init(GPIOC, &GPIO_InitStruct);
	//also set PinSource to 4
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource3);
	
	//once again line 4
	EXTI_InitStruct.EXTI_Line = EXTI_Line3;
	EXTI_InitStruct.EXTI_LineCmd = ENABLE;
	EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
	//could get away with EXTI_Trigger_Rising or EXTI_Trigger_Falling
	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
	EXTI_Init(&EXTI_InitStruct);
	
	
	NVIC_InitStruct.NVIC_IRQChannel = EXTI3_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0x00;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0x00;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);
	
	EXTI_ClearITPendingBit(EXTI_Line3);
}

/**
 * Configure the RTC to operate based on the LSI (Internal Low Speed oscillator)
 * and configure one of the alarms (A or B) to trigger an external interrupt every second
 * (e.g. EXTI line 17 for alarm A).
 */
void RTC_Config(void){
	/* Enable write access to the RTC ****************************/
/* Enable the PWR clock */
RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
/* Allow access to RTC */
PWR_BackupAccessCmd(ENABLE);
/* Configure the RTC clock source ****************************/
/* Enable the LSE OSC */
RCC_LSEConfig(RCC_LSE_ON);
/* Wait till LSE is ready */
while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
{}
/* Select the RTC Clock Source */
RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
/* Enable the RTC Clock */
RCC_RTCCLKCmd(ENABLE);
/* Wait for RTC APB registers synchronisation */
RTC_WaitForSynchro();
/* Configure the RTC calendar, Time and Date *****************/
/* RTC time base = LSE / ((AsynchPrediv+1) * (SynchPrediv+1))
= 1 Hz
*/
RTC_InitStructure.RTC_AsynchPrediv = 127;
RTC_InitStructure.RTC_SynchPrediv = 255;
RTC_InitStructure.RTC_HourFormat = RTC_HourFormat_24;
RTC_Init(&RTC_InitStructure);
/* Set the Time */
RTC_TimeStructure.RTC_Hours = 0x00;
RTC_TimeStructure.RTC_Minutes = 0x00;
RTC_TimeStructure.RTC_Seconds = 0x00;
RTC_SetTime(RTC_Format_BCD, &RTC_TimeStructure);
/* Set the Date */
RTC_DateStructure.RTC_Month = RTC_Month_May;
RTC_DateStructure.RTC_WeekDay = RTC_Weekday_Monday;
RTC_DateStructure.RTC_Date = 0x30;
RTC_DateStructure.RTC_Year = 0x11;
RTC_SetDate(RTC_Format_BCD, &RTC_DateStructure);
}
