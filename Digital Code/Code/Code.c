/*
 * max_adc.c
 * Obtain ADC value and control corresponding servo movement using PWM
 * Created: 3/23/2014 12:00:53 PM
 *  Author: DinEsH
 */ 
# define F_CPU 2000000UL
#include <avr/io.h>
#include <stdlib.h>
#include <util/delay.h>
#include <stdint.h>
#include <math.h>

int i,j,k;
uint32_t adc_mean,adc_max;

int main(void)
{	
	
	CLKSEL0=0x35;//  EXSUT1:0=10 ...ALSO ENABLE LOW POWER CRYSTAL AND SELECT CRYSTAL
	CLKSEL1=0x0F; // EXCKSEL3:0= 1111 ;
	UHWCON = 0x01; // USB PAD REGULATOR ENABLE.

	//General configuration
	DDRB=0xFF;
	DDRD=0xFF;
	
	//configure  ADC
	ADCSRA=0x87;//enable adc and prescalar 128
	DIDR0=0xf3;
	DIDR2=0x3f;
	ADMUX=0xC1;//internal vref, RIGHT ADJUST and adc1
	
	//Configure TIMER1
	TCCR1A|=(1<<COM1A1)|(1<<COM1B1)|(1<<WGM11);        //NON Inverted PWM
	TCCR1B|=(1<<WGM13)|(1<<WGM12)|(1<<CS11)|(1<<CS10); //PRESCALER=64 MODE 14(FAST PWM)
	ICR1=624;										   //fPWM=50Hz (Period = 20ms Standard).
	DDRB=0xFF;										   //PWM Pins as Out
	int sendByte;
	//Configure Serial
	UCSR1B=0x18;									   // Turn on the transmission and reception circuitry
	UCSR1C=0x06;									   // Use 8- bit character sizes
	UBRR1= 12;										   //BAud Rate 9600
	while (1)
	{	
		//collect ADC values
		adc_max=0;
		for (j=0;j<100;j++)
		{
			//Labview Data
			sendByte=ADC;
			UCSR1B=0x08;
			while (( UCSR1A & (1 << UDRE1 )) == 0) {};
			// Do nothing until UDR is ready for more data to be written to it
			UDR1 =sendByte;
			
			for (i=0;i<255;i++)
			{
				ADCSRA= 0xc7;						    //ADCSRA | 0x40;//ENABLE,SOC AND PRESCALAR
				_delay_us(20);
				while (!ADIF);
				if (adc_max<ADC)
				{
					adc_max=ADC;
				}	
			}
		}
		adc_mean=adc_max;
		//PWM
		if(adc_mean>0x118)	// 0.7V
		{
			OCR1A=71;	
			for(int i=0;i<3;i++)
			_delay_ms(800);			//180 degree
		}
		else if(adc_mean>0xB4)	// 0.450V
		{
			OCR1A=47;				//120 degree
			_delay_ms(800);
		}
		else if(adc_mean>0x80) //0.25V
		{
				OCR1A=20;	
			_delay_ms(800);			//45 degree
		}
		else
		{
			
		}
			OCR1A=14;				//0   degree		
	};		
}