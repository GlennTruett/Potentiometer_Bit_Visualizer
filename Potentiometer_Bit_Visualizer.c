/*
 * GccApplication30.c
 *
 * Created: 5/8/2024 4:45:54 PM
 * Author : Glenn
 */ 

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

void ADC_init() {
	ADMUX |= (0 << REFS1) | (1 << REFS0); // Use AVCC as reference voltage
	ADCSRA |= (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // Enable ADC, set prescaler to 128
	ADCSRB = 0x00;
	DIDR0 |= (1 << ADC0D); // Disable digital input on ADC0 pin
}

uint16_t ADC_read() {
	ADCSRA |= (1 << ADSC); // Start ADC conversion
	while (ADCSRA & (1 << ADSC)); // Wait for conversion to complete
	return ADC; // Return ADC value
}

int main(void)
{
	DDRB |= (1 << PB1); // Set PB1 as output for PWM LED

	// Set Timer1 for PWM
	TCCR1A |= (1 << COM1A1) | (1 << WGM10); // Fast PWM, non-inverting mode
	TCCR1B |= (1 << WGM12) | (1 << CS11); // Prescaler of 8
	OCR1A = 0; // Set initial PWM duty cycle to 0
	
	ADC_init(); // Initialize ADC
	
	while (1)
	{
		// Set Timer0 for blinking PB3
		DDRB |= (1 << PB3); // Set PB3 as output for blinking LED
		// Set Timer0 for CTC mode
		TCCR0A |= (1 << WGM01);
		// Set Timer0 prescaler to 1024
		TCCR0B |= (1 << CS02) | (1 << CS00);
		// Set compare value for approximately 1 second delay
		OCR0A = 156;

		// Enable external interrupt INT0
		EICRA |= (1 << ISC01) | (1 << ISC00); // Trigger on rising edge
		EIMSK |= (1 << INT0); // Enable external interrupt INT0

		sei(); // Enable global interrupts

	
		// PB1 blinking with PWM control
		// Adjust brightness based on potentiometer
		uint16_t potValue = ADC_read(); // Read ADC value from potentiometer
		OCR1A = potValue >> 2; // Adjust PWM duty cycle (potValue ranges from 0 to 1023, but OCR1A ranges from 0 to 255)
		_delay_ms(500); // Adjust the delay for blinking speed
		
		PORTB ^= (1 << PB3);
		_delay_ms(200);
	}
	return 0;
}

