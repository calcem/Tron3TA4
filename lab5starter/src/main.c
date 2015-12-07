#include "main.h"


void GPIO_Config(void);
void TimerConfig(void);
void ExtPB_Config(void);
void ExtPBNum2(void);
uint16_t PrescalerValue = 0;
__IO uint8_t UBPressed = 0;
__IO uint8_t EB1Pressed = 0;
__IO uint8_t EB2Pressed = 0;
int pushbuttonState = 1;
int EB1State = 0;
int EB2State = 0;

int main(void){
	PrescalerValue = (uint16_t) ((SystemCoreClock / 2) / 500000) - 1;
	GPIO_Config();
	TimerConfig();
	ExtPB_Config();
	ExtPBNum2();
	STM_EVAL_PBInit(BUTTON_USER, BUTTON_MODE_EXTI);
	
	  LCD_Init();
  
  /* LCD Layer initiatization */
  LCD_LayerInit();
    
  /* Enable the LTDC */
  LTDC_Cmd(ENABLE);
  
  /* Set LCD foreground layer */
  LCD_SetLayer(LCD_FOREGROUND_LAYER);
	
	LCD_DisplayStringLine(LINE(0),  (uint8_t *) "                       ");
			LCD_DisplayStringLine(LINE(1),  (uint8_t *) "                   ");
	
	LCD_DisplayStringLine(LINE(2),  (uint8_t *) "                       ");
	
	LCD_DisplayStringLine(LINE(0),  (uint8_t *) "Full Stepping"); 
	
	LCD_DisplayStringLine(LINE(1),  (uint8_t *) "Clockwise"); 
	
	
	LCD_DisplayStringLine(LINE(2),  (uint8_t *) "Same Speed");
	while(1){
		if(UBPressed == 1 && pushbuttonState == 0){
			LCD_DisplayStringLine(LINE(0),  (uint8_t *) "                   ");
			LCD_DisplayStringLine(LINE(1),  (uint8_t *) "                   ");
			LCD_DisplayStringLine(LINE(0),  (uint8_t *) "Full Stepping");
			LCD_DisplayStringLine(LINE(1),  (uint8_t *) "Clockwise"); 			
			STM_EVAL_PBInit(BUTTON_USER, BUTTON_MODE_EXTI);
			UBPressed = 0;
			pushbuttonState = 1;
		}
		if(UBPressed == 1 && pushbuttonState == 1){
			LCD_DisplayStringLine(LINE(0),  (uint8_t *) "                   ");
			LCD_DisplayStringLine(LINE(1),  (uint8_t *) "                   ");
			LCD_DisplayStringLine(LINE(0),  (uint8_t *) "Half Stepping"); 
			LCD_DisplayStringLine(LINE(1),  (uint8_t *) "Clockwise"); 
			STM_EVAL_PBInit(BUTTON_USER, BUTTON_MODE_EXTI);
			UBPressed = 0;
			pushbuttonState = 2;
		}
		
		if(UBPressed == 1 && pushbuttonState == 2){
			LCD_DisplayStringLine(LINE(0),  (uint8_t *) "                   ");
			LCD_DisplayStringLine(LINE(1),  (uint8_t *) "                   ");
			LCD_DisplayStringLine(LINE(0),  (uint8_t *) "Full Stepping"); 
			LCD_DisplayStringLine(LINE(1),  (uint8_t *) "CounterClockwise"); 
			STM_EVAL_PBInit(BUTTON_USER, BUTTON_MODE_EXTI);
			UBPressed = 0;
			pushbuttonState = 3;
		}
				
		if(UBPressed == 1 && pushbuttonState == 3){
			LCD_DisplayStringLine(LINE(0),  (uint8_t *) "                   ");
			LCD_DisplayStringLine(LINE(1),  (uint8_t *) "                   ");
			LCD_DisplayStringLine(LINE(0),  (uint8_t *) "Half Stepping"); 
			LCD_DisplayStringLine(LINE(1),  (uint8_t *) "CounterClockwise"); 
			STM_EVAL_PBInit(BUTTON_USER, BUTTON_MODE_EXTI);
			UBPressed = 0;
			pushbuttonState = 0;
		}
		
		
		
		
		
		
		
		
		if(EB1Pressed == 1 && EB1State == 0){
			LCD_DisplayStringLine(LINE(2),  (uint8_t *) "                 ");
			LCD_DisplayStringLine(LINE(2),  (uint8_t *) "Decrease Speed"); 
			
			ExtPB_Config();
			EB1Pressed = 0;
			EB1State = 1;
		}
		
		if(EB1Pressed == 1 && EB1State == 1){
			LCD_DisplayStringLine(LINE(2),  (uint8_t *) "                   ");
			LCD_DisplayStringLine(LINE(2),  (uint8_t *) "Same Speed"); 
			ExtPB_Config();
			EB1Pressed = 0;
			EB1State = 0;
		}
		
		if(EB2Pressed == 1 && EB2State == 0){
			LCD_DisplayStringLine(LINE(2),  (uint8_t *) "                   ");
			LCD_DisplayStringLine(LINE(2),  (uint8_t *) "Increase Speed"); 
			ExtPBNum2();
			EB2Pressed = 0;
			EB2State = 1;
		}
		
		if(EB2Pressed == 1 && EB2State == 1){
			LCD_DisplayStringLine(LINE(2),  (uint8_t *) "                   ");
			LCD_DisplayStringLine(LINE(2),  (uint8_t *) "Same Speed"); 
			ExtPBNum2();
			EB2Pressed = 0;
			EB2State = 0;
		}
		
	}
}

/**
 * Configure the GPIO for output to the motor.
 */
void GPIO_Config(void){

}


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
 * Configure the TIM3 in output compare mode.
 */
void TimerConfig(void) 
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

