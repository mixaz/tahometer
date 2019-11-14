#include "iostm8s003f3.h"

volatile unsigned long pulses = 0;

#define STRINGVECTOR(x) #x
#define ISR( a, b ) \
_Pragma( STRINGVECTOR( vector = (b) ) ) \
__interrupt void (a)( void )

void InitLED();
void InitTIM1();

ISR(PortB_interrupt, EXTI1_vector)
{
  pulses++;
}

int main(void){
  CLK_CKDIVR = 0x00;  //CPUDIV = 1 HSIDIV = 1 internal clock = 16 Mhz
  asm("sim");         //First close the total interrupt
  InitLED();          //Various initializations
  InitTIM1();   
  asm("rim");         //Open total interrupt
  while(1) ;          //Enter the dead cycle and wait for the timer cycle to interrupt
}


/*********************************************************

Timer 1 updates interrupt processing function
 Interrupt vectors can also be replaced by decimal system, which is used here for convenience.
Because the macro definition of the header file is in hexadecimal.
Function names can be written freely, just be happy.

*****************************************************/
#pragma vector = 0x0D    //This is the interrupt format, just call it directly.
__interrupt void TIM1_OVR_UIF(void)
{
  TIM1_SR1 &= ~(1<<0);//Clear interruption mark

  PD_ODR_bit.ODR2 = (pulses >= 60 ? 1 : 0);
  PD_ODR_bit.ODR3 = (pulses >= 70 ? 1 : 0);

  pulses = 0;
}

/*********************************************

Timer 1 initialization
tim1 Downward counting mode,

************************************************/
void InitTIM1()
{
  TIM1_PSCRH=0xC3;//Be sure to write high eight first
  TIM1_PSCRL=0x50;//50000, 16Mhz/50000=320Hz

  TIM1_ARRH=0X01;//We must first install eight high positions and then eight low positions.
  TIM1_ARRL=0X40;//320 - reload each 1 sec

  TIM1_CNTRH=0;
  TIM1_CNTRL=0;//It is necessary to clear down the counter

  TIM1_IER |= 1<<0;//Enable tim1 update interrupt
  TIM1_SR1 |= 1<<0;//Clear tim1 update interrupt flag

  TIM1_CR1 |= 1<<7;//Allow reassembly to enable timer
  TIM1_CR1 |= 1<<4;//Select Downward Counting Mode
  TIM1_CR1 |= 1<<0;//Enabling counter
}


/******************************************

LED Initialization

********************************************/
void InitLED()
{
    PD_DDR |= 1<<2;//PD2 directional output
    PD_CR1 |= 1<<2;
    PD_CR2 |= 1<<2;//Maximum output speed is 10M
    PD_ODR |= 1<<2;//PD2 output 1
    PD_DDR |= 1<<3;//PD3 directional output
    PD_CR1 |= 1<<3;//PD3is push-pull output
    PD_CR2 |= 1<<3;//Maximum output speed is 10M
    PD_ODR |= 1<<3;//PD3 output 1
    PD_ODR = 0<<3;//LED lamp flipped once in 1s
    PD_ODR = 0<<2;//LED lamp flipped once in 1s
   
    ///////////////rx
    PB_DDR_bit.DDR4 = 0; //PB4 — as input
    PB_CR1_bit.C14 = 1; //sos pulup
    PB_CR2_bit.C24 = 1; // inerep 1

    EXTI_CR1_bit.PBIS = 2;
};


