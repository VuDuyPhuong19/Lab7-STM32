#include "ADC.h"
#include "stm32f10x.h"

void ADC_Init(void){ uint32_t delay;
	// Enable ADC clock
	RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
	// Configure GPIO Port A 
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
	delay = RCC->APB2ENR;
	// Set A0 is analog input
	GPIOA->CRL &= ~(GPIO_CRL_MODE0 | GPIO_CRL_CNF0);  // A0
	
	// Set ADC prescaler to 6 (ADC clock = 72MHz / 6 = 12 MHz)
	RCC->CFGR = RCC_CFGR_ADCPRE_DIV6; 
	ADC1->CR2 &= ~ADC_CR2_ADON;
	
	// Configure ADC channel
	ADC1->SQR1  = 0x0;
	ADC1->SQR3 = 0x0;
	
	ADC1->SMPR2 &= ~ADC_SMPR2_SMP0_2;
	ADC1->SMPR2 |= ADC_SMPR2_SMP0_2; // Sample time , 41,5 x 1/12M
	
	// Starts conversion of the regular channel
	ADC1->CR2 &= ADC_CR2_SWSTART;
	ADC1->CR2 |= ADC_CR2_SWSTART;
	
	ADC1->CR2 &= ~ADC_CR2_CONT;
	ADC1->CR2 |= ADC_CR2_CONT; // Enable Continous conversion mode
	ADC1->CR2 |= ADC_CR2_ADON; // Enable ADC 	
	
	while (!(ADC1->CR2 & ADC_CR2_ADON)){} // Wait for the ADC to complete initialization
		
	NVIC->ISER[0] = 1 << ADC1_2_IRQn;
	ADC1->CR1 |= ADC_CR1_EOCIE; // B?t ng?t EOC cho ADC1
}

uint32_t ADC_In(void){
	while(!(ADC1->SR &ADC_SR_EOC)){} // Wait for conversion to complete
	uint16_t data = ADC1->DR & 0x0FFF;
	return data;
}
void ADC1_2_IRQHandler(void) {
  if ((ADC1->SR & ADC_SR_EOC) != 0) {
    ADC1->SR &= ~ADC_SR_EOC;
  }
}
