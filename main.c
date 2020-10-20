
#define F_CPU 1000000UL
#include <avr/io.h>
#include "lcd/lcd.h"
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h>

static volatile int pulse = 0;
static volatile int EC = 0;



int main (void)
{
	int count_a = 0;
	char show_a[16];
	DDRD |= (1<<0);   //Making trigger pin as output
	DDRD &= ~(1<<2); //Making echo pin as input
	DDRA |= (1<<7);  //For output of first IR sensor 
	DDRB |= (1<<0); //Four output of second IR sensor
	
	_delay_ms(50);
	lcd_init(LCD_DISP_ON_CURSOR);
	lcd_clrscr();
	
	

	DDRC=0xff; // To make motors pin as output pins
	PORTC=0x00; // to start the motor with initial value of 0


	GICR |=  (1<<INT0);
	MCUCR |= (1<<ISC00);
	sei();

	while (1)
	{

		PORTD &= ~(1<<PIND0);  //disable the trigger pin
		_delay_us(2);
		
		PORTD |= 1<<PIND0; //enable the trigger pin this will send pulse from the transmitter to the object then the reciver will recive it and calculate the distance
		_delay_us(10);

		PORTD &= ~(1<<PIND0);
		
		count_a = pulse/58;  //You can calculate the range through the time interval between sending trigger signal and receiving echo signal. Formula: uS / 58 = centimeters

		lcd_home();
		lcd_puts("Distance Sensor");
		lcd_gotoxy(0,1);
		lcd_puts("Distance=");
		
		float x=0+(100-0)*(count_a-190)/(320-190); //mapping

		itoa(x,show_a,10);
		lcd_puts(show_a);
		lcd_puts(" ");
		lcd_gotoxy(13,2);
		lcd_puts("cm");
		lcd_gotoxy(1,1);
		
//Initial case is that the car is moving forward in case of the ultrasonic and both IR sensor did not detect anything	
		PORTC |= (1<<0);
		PORTC |= (1<<2);
		PORTC |= (1<<4);
		PORTC |= (1<<6);

		PORTC &= ~(1<<1);
		PORTC &= ~(1<<3);
		PORTC &= ~(1<<5);
		PORTC &= ~(1<<7);		
/* 
Second case is that the ultrasonic detected something and the range between the car and the object is between 5 and 20 cm 
then the car will look at the first the left and Right IR if both are giving one the car will stop 
if only one of them giving one the car will move right or left depending on which IR is on
*/	
		if((x>=5) && (x<=20))
		{
			while ((PINA &=(1<<7)) && (PINB &=(1<<0)))
			{
		PORTC &= ~(1<<0);
		PORTC &= ~(1<<1);
		PORTC &= ~(1<<2);
		PORTC &= ~(1<<3);
		PORTC &= ~(1<<4);
		PORTC &= ~(1<<5);
		PORTC &= ~(1<<6);
		PORTC &= ~(1<<7);				
			}
			if(( PINA &=(1<<7)))
			{
		PORTC |= (1<<0);
		PORTC |= (1<<4);
		

		PORTC &= ~(1<<1);
		PORTC &= ~(1<<2);
		PORTC &= ~(1<<3);
		PORTC &= ~(1<<5);
		PORTC &= ~(1<<6);
		PORTC &= ~(1<<7);				
			}
			if(( PINB &=(1<<0)))
			{
		PORTC |= (1<<3);
		PORTC |= (1<<7);
		

		PORTC &= ~(1<<0);
		PORTC &= ~(1<<1);
		PORTC &= ~(1<<2);
		PORTC &= ~(1<<4);
		PORTC &= ~(1<<5);
		PORTC &= ~(1<<6);			
		  }	
	}
/*
Third case if the left IR detected black line the car will move to the right 
 
*/
	if (PINA &= (1<<7))
	{

		PORTC |= (1<<0);
		PORTC |= (1<<4);
		

		PORTC &= ~(1<<1);
		PORTC &= ~(1<<2);
		PORTC &= ~(1<<3);
		PORTC &= ~(1<<5);
		PORTC &= ~(1<<6);
		PORTC &= ~(1<<7);			
		}
	
/*
The forth case is like third case but for the right IR sensor
*/
	if (PINB &= (1<<0))
	{

		   PORTC |= (1<<3);
		   PORTC |= (1<<7);
		
		   PORTC &= ~(1<<0);
		   PORTC &= ~(1<<1);
		   PORTC &= ~(1<<2);
		   PORTC &= ~(1<<4);
		   PORTC &= ~(1<<5);
		   PORTC &= ~(1<<6);
		}
	}	

		}

ISR(INT0_vect)  //Interrupt for the ultrasonic sensor
{

	if (EC==1)
	{
		TCCR1B = 0;
		pulse = TCNT1;
		TCNT1 = 0;
		EC = 0;
	}
	else if(EC == 0)
	{
		TCCR1B |= (1<<CS10);
		EC = 1;
	}
}

