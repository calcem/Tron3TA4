/**
  ******************************************************************************
  * @file    Lab2/main.c
  * @author  Robert Li, modified by Keybo Q.
  * @version 
  * @date    August 2014
  * @brief   this starter project demonstrates: 
						1. timer configuration
						2. timer output compare interrupt set up
						3. LCD configuration
						4. EEPROM emulator config
						5. RNG config
	* @projectfunction  pressing the RESET BUTTON:some strings, chars, and numbers will be displayed on LCD, and LED3 will be on, LED4 will blink
											Each time when the USER BUTTON is pressed:  
												1. LED3 will toggle 
												2. LED4 will toggle the blink rate
												3. "hello world" will move down one line
												4. a new random number will be displayed on line 12
												5  an increasing count number written to and read from EEPROM will be displayed on line 13, the last line.
  
  ******************************************************************************
 * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "main.h" 




/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define COLUMN(x) ((x) * (((sFONT *)LCD_GetFont())->Width))    //see font.h, for defining LINE(X)




/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
TIM_OCInitTypeDef  TIM_OCInitStructure;
__IO uint16_t CCR1_Val = 50000;
uint16_t PrescalerValue = 0;
__IO uint8_t UBPressed = 0;

int i;
uint16_t line;

char lcd_buffer[14];    // LCD display buffer


/* Virtual address defined by the user: 0xFFFF value is prohibited
 * This global variable stores the EEPROM addresses for NB_OF_VAR(=3) variables
 * where the user can write to. To increase the number of variables written to EEPROM
 * modify this variable and NB_OF_VAR in eeprom.h
 */
uint16_t VirtAddVarTab[NB_OF_VAR] = {0x5555, 0x6666, 0x7777};
uint16_t VarDataTab[NB_OF_VAR] = {0, 0, 0};
uint16_t VarValue = 10000;

/* Private function prototypes -----------------------------------------------*/
void PB_Config(void);
void LED_Config(void);
void TIM3_Config(void);
void TIM3_OCConfig(void);

void LCD_DisplayString(uint16_t LineNumber, uint16_t ColumnNumber, uint8_t *ptr);
void LCD_DisplayInt(uint16_t LineNumber, uint16_t ColumnNumber, int Number);
void LCD_DisplayFloat(uint16_t LineNumber, uint16_t ColumnNumber, float Number, int DigitAfterDecimalPoint);
void RNG_Config(void);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program
  * @param  None
  * @retval None


  */



int counter=1;

//0 = blinking
//1 = 2 second wait
//2 = off
int toggle = 0;

int rand = 0;

int randomNumber(void){
		rand = RNG_GetRandomNumber()/120000;
		if(rand > 6000 && rand < 12000){
			return rand;
		}else{
			randomNumber();
		}
}


void resetTimer(void){
				TIM_Cmd(TIM3, DISABLE);
				TIM_ITConfig(TIM3, TIM_IT_CC1, DISABLE);
				TIM_PrescalerConfig(TIM3, 6000, TIM_PSCReloadMode_Immediate);
				TIM_ITConfig(TIM3, TIM_IT_CC1, ENABLE);
				TIM_Cmd(TIM3, ENABLE);
}

void resetTimerLong(void){
				TIM_Cmd(TIM3, DISABLE);
				TIM_ITConfig(TIM3, TIM_IT_CC1, DISABLE);
				TIM_PrescalerConfig(TIM3, 50000, TIM_PSCReloadMode_Immediate);
				TIM_ITConfig(TIM3, TIM_IT_CC1, ENABLE);
				TIM_Cmd(TIM3, ENABLE);
}

		char str[15];

int main(void)
{
  /*!< At this stage the microcontroller clock setting is already configured, 
       this is done through SystemInit() function which is called from startup
       file (startup_stm32f4xx.s) before to branch to application main.
       To reconfigure the default setting of SystemInit() function, refer to
       system_stm32f4xx.c file
     */
	
//initiate user button
  PB_Config();

	//initiate LEDs and turn them on
  LED_Config();	
	
 

  /* -----------------------------------------------------------------------
    TIM3 Configuration: Output Compare Timing Mode:
    
    In this example TIM3 input clock (TIM3CLK) is set to 2 * APB1 clock (PCLK1), 
    since APB1 prescaler is different from 1.   
      TIM3CLK = 2 * PCLK1  
      PCLK1 = HCLK / 4 
      => TIM3CLK = HCLK / 2 = SystemCoreClock /2
          
    To get TIM3 counter clock at 50 MHz, the prescaler is computed as follows:
       Prescaler = (TIM3CLK / TIM3 counter clock) - 1
       Prescaler = ((SystemCoreClock /2) /0.5 MHz) - 1
                                              
    CC1 update rate = TIM3 counter clock / CCR1_Val = 10.0 Hz
    ==> Toggling frequency = 5 Hz

    Note: 
     SystemCoreClock variable holds HCLK frequency and is defined in system_stm32f4xx.c file.
     Each time the core clock (HCLK) changes, user had to call SystemCoreClockUpdate()
     function to update SystemCoreClock variable value. Otherwise, any configuration
     based on this variable will be incorrect.    
		 ----------------------------------------------------------------------- */ 	
	
	//=======================Configure and init Timer======================
  /* Compute the prescaler value */
  PrescalerValue = (uint16_t) ((SystemCoreClock / 2) / 500000) - 1;

 /* TIM Configuration */
  TIM3_Config();

	// configure the output compare
	TIM3_OCConfig();

  /* TIM Interrupts enable */
  TIM_ITConfig(TIM3, TIM_IT_CC1, ENABLE);

  /* TIM3 enable counter */
  TIM_Cmd(TIM3, ENABLE);
	
//======================================configure and init LCD  ======================	
	 /* LCD initiatization */
  LCD_Init();
  
  /* LCD Layer initiatization */
  LCD_LayerInit();
    
  /* Enable the LTDC */
  LTDC_Cmd(ENABLE);
  
  /* Set LCD foreground layer */
  LCD_SetLayer(LCD_FOREGROUND_LAYER);
	
//================EEPROM init====================================

/* Unlock the Flash Program Erase controller */
		FLASH_Unlock();
		/* EEPROM Init */
		EE_Init();
		

//============ Set up for random number generation==============
	RNG_Config();


	//with the default font, LCD can display  12 lines of chars, they are LINE(0), LINE(1)...LINE(11) 
	//with the default font, LCD can display  15 columns, they are COLUMN(0)....COLUMN(14)


		LCD_Clear(LCD_COLOR_WHITE);
			
	

		
		LCD_DisplayStringLine(LINE(0),  (uint8_t *) "Attempt");
		LCD_DisplayStringLine(LINE(2),  (uint8_t *) "Record");
		
		EE_WriteVariable(VirtAddVarTab[0],VarValue);
		EE_ReadVariable(VirtAddVarTab[0], &VarDataTab[0]);
		sprintf(str, "%d", VarDataTab[0]);
		//LCD_DisplayStringLine(LINE(3),  (uint8_t *) str);
		//randomNumber = RNG_GetRandomNumber()/100000;

		//sprintf(str, "%d", randomNumber());
		//LCD_DisplayStringLine(LINE(5),  (uint8_t *) str);
		
		resetTimer();
		
		/*the following while loop is where the main part of the code is
		* it currently uses the userbutton on board since Mario forgot to bring along his
		* jumper cables to test out the push button part
		*/
		
		//if toggle = 0 lights are  blinking
		//if toggle = 1 2 second wait
		//if toggle = 2 LED toggle off, the lights stay on
		
		//@TODO add external push button to code
  while (1){ 
			int num = TIM_GetCounter(TIM3);
		//This is for the start of the procedure 
		if(toggle==0){
			if(num == 3000){
					STM_EVAL_LEDOn(LED3);
				  STM_EVAL_LEDOn(LED4);
			}
			else if(num == 6000){
					STM_EVAL_LEDOff(LED3);
					STM_EVAL_LEDOff(LED4);
					resetTimer();
			}
		}
			//if the user button has been pressed and the lights are blinking
			if (UBPressed==1 && toggle==0) {
				STM_EVAL_LEDOff(LED3);
				STM_EVAL_LEDOff(LED4);
				UBPressed=0;
				PB_Config();
				resetTimerLong();
				toggle = 1;
				rand = randomNumber();//generate a random number 
			}

			//this is the to get the wait time for the reaction test.
			if(toggle==1){
					if(num == rand){ //if num is equal to the ramdom gened number turn on the LEDs and reset the timer
							STM_EVAL_LEDOn(LED3);
							STM_EVAL_LEDOn(LED4);
							resetTimerLong();
					}
			}
			//this is the code for when the reaction timer has gone off
			if (UBPressed==1 && toggle==1) {
				//this block of code (from line 281 -284) writes to the LCD the lastest user reaction time. 
				sprintf(str, "%d", num);
				LCD_DisplayStringLine(LINE(1),  (uint8_t *) "          ");
				LCD_DisplayStringLine(LINE(1),  (uint8_t *) str);
				EE_ReadVariable(VirtAddVarTab[0], &VarDataTab[0]);
				//this if statement determines wheter the user has beat their best reaction time
				if(num < VarDataTab[0]){
						VarValue = num;
						EE_WriteVariable(VirtAddVarTab[0],VarValue);
				}
				/*the following block of code writes to the LCD the record reaction time*/
				EE_ReadVariable(VirtAddVarTab[0], &VarDataTab[0]);
				sprintf(str, "%d", VarDataTab[0]);
				LCD_DisplayStringLine(LINE(3),  (uint8_t *) "          ");
				LCD_DisplayStringLine(LINE(3),  (uint8_t *) str);
				UBPressed=0;
				PB_Config();
				resetTimerLong();
				toggle = 2; 
			}
			//the user needs to press the button to get the reaction time game going again. 
			//to reset the reaction timer
			if (UBPressed==1 && toggle==2) {
				UBPressed=0;
				PB_Config();
				resetTimer();
				toggle = 0;
			}
			
	}
	
}

/**
  * @brief  Configure the TIM IRQ Handler.
  * @param  No
  * @retval None
  */


void PB_Config(void)
{
/* Initialize User_Button on STM32F4-Discovery
   * Normally one would need to initialize the EXTI interrupt
   * to handle the 'User' button, however the function already
   * does this.
   */
  STM_EVAL_PBInit(BUTTON_USER, BUTTON_MODE_EXTI);
}

void LED_Config(void)
{
 /* Initialize Leds mounted on STM32F429-Discovery board */
  STM_EVAL_LEDInit(LED3);
	STM_EVAL_LEDInit(LED4); 

  /* Turn on LED3, LED4 */
  STM_EVAL_LEDOn(LED3);
	STM_EVAL_LEDOn(LED4);
}


void TIM3_Config(void) 
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	//since TIMER 3 is on APB1 bus, need to enale APB1 bus clock first
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
	
	//====================================================
	//Enable TIM3 global interrupt ====does this part need to be done before TIM_BaseStructure set up?
	NVIC_InitStructure.NVIC_IRQChannel=TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0X00;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x01;
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);	
	//================================================
	
	TIM_TimeBaseStructure.TIM_Period=65535; // need to be larger than CCR1_VAL, has no effect on the Output compare event.
	TIM_TimeBaseStructure.TIM_Prescaler=PrescalerValue;    //why all the example make this one equal 0, and then use 
					//function TIM_PrescalerConfig() to re-assign the prescaller value?
	TIM_TimeBaseStructure.TIM_ClockDivision=0;
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;	
	 
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

	//TIM_PrescalerConfig(TIM3, TIM3Prescaler, TIM_PSCReloadMode_Immediate);
}


void TIM3_OCConfig(void) {
	TIM_OCInitTypeDef TIM_OCInitStructure;
	
	TIM_OCInitStructure.TIM_OCMode=TIM_OCMode_Timing;
	TIM_OCInitStructure.TIM_OutputState=TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse=CCR1_Val;
	TIM_OCInitStructure.TIM_OCPolarity=TIM_OCPolarity_High;
	
	TIM_OC1Init(TIM3, &TIM_OCInitStructure);
	
	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Disable); //if disabled, 
	//the TIMx_CCRx register can be updated at any time by software to control the output
	//waveform---from the reference manual
}	
	



void TIM_Config(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;

  /* TIM3 clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

  /* Enable the TIM2 global Interrupt
   * THis is used to capture the button push.
   */
  NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

}


void LCD_DisplayString(uint16_t LineNumber, uint16_t ColumnNumber, uint8_t *ptr)
{  
  //here the LineNumber and the ColumnNumber are NOT  pixel numbers!!!
		while (*ptr!=NULL)
    {
				LCD_DisplayChar(LINE(LineNumber), COLUMN(ColumnNumber), *ptr);
				ColumnNumber++;
			 //to avoid wrapping on the same line and replacing chars 
				if (ColumnNumber*(((sFONT *)LCD_GetFont())->Width)>=LCD_PIXEL_WIDTH ){
					ColumnNumber=0;
					LineNumber++;
				}
					
				ptr++;
		}
}

void LCD_DisplayInt(uint16_t LineNumber, uint16_t ColumnNumber, int Number)
{  
  //here the LineNumber and the ColumnNumber are NOT  pixel numbers!!!
		char lcd_buffer[15];
		sprintf(lcd_buffer,"%d",Number);
	
		LCD_DisplayString(LineNumber, ColumnNumber, (uint8_t *) lcd_buffer);
}

void LCD_DisplayFloat(uint16_t LineNumber, uint16_t ColumnNumber, float Number, int DigitAfterDecimalPoint)
{  
  //here the LineNumber and the ColumnNumber are NOT  pixel numbers!!!
		char lcd_buffer[15];
		
		sprintf(lcd_buffer,"%.*f",DigitAfterDecimalPoint, Number);  //6 digits after decimal point, this is also the default setting for Keil uVision 4.74 environment.
	
		LCD_DisplayString(LineNumber, ColumnNumber, (uint8_t *) lcd_buffer);
}

void RNG_Config(void){
	//Enable RNG controller clock
	RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_RNG, ENABLE);
	
	//activate the RNG peripheral 
	RNG_Cmd(ENABLE);
	
	// to get a random number, need to continue steps: 3. Wait until the 32 bit Random number Generator 
	//contains a valid random data (using polling/interrupt mode). For more details, 
	//refer to Section 20.2.4: "Interrupt and flag management" module description.
	//4. Get the 32 bit Random number using RNG_GetRandomNumber() function
	//5. To get another 32 bit Random number, go to step 3.
}	




#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  while (1)
  {}
}
#endif

/**
  * @}
  */ 

/**
  * @}
  */ 

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
