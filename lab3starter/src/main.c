#include "main.h"

//memory location to write to in the device
__IO uint16_t memLocation = 0x000A; //pick any location within range

uint8_t Tx1_Buffer = 'A';
uint8_t Rx1_Buffer;
uint16_t NumDataRead = 1;

uint16_t line;
		char time[15];
		int hours;
		int minutes;
		int seconds;
		
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
	
	
	//main program
	
	LCD_Clear(LCD_COLOR_WHITE);
		
	//line=0;
	//Display a string in one line, on the first line (line=0)
	//LCD_DisplayStringLine(LINE(line),  (uint8_t *) "Init EEPROM...");
	//line++;
	
	//i2c_init(); //initialize the i2c chip
	//sEE_Init();  

	
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
	//	LCD_DisplayStringLine(LINE(line),  (uint8_t *) "Success!");  
	//}else{
	//	LCD_DisplayStringLine(LINE(line),  (uint8_t *) "Mismatch!"); 
	//}
	
	//main loop
	while(1){
		RTC_GetTime(RTC_Format_BIN,&RTC_TimeStructure);
		hours = RTC_TimeStructure.RTC_Hours;
		minutes = RTC_TimeStructure.RTC_Minutes;
		seconds = RTC_TimeStructure.RTC_Seconds;

		sprintf(time,"%d : %d : %d",hours,minutes,seconds);
		LCD_DisplayStringLine(LINE(0),  (uint8_t *) time); 
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
