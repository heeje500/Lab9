/*	Author: lab
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #  Exercise #
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

void set_PWM(double frequency) {
	static double current_frequency;

	if (frequency != current_frequency) {
		if (!frequency) {
			TCCR3B &= 0x08;
		}
		else {
			TCCR3B |= 0x03;
		}

		if (frequency < 0.954) {
			OCR3A = 0xFFFF;
		}

		else if (frequency > 31250) {
			OCR3A = 0x0000;
		}

		else {
		OCR3A = (short) (8000000 / (128 * frequency)) - 1;
		}
		TCNT3 = 0;
		current_frequency = frequency;
	}
}


void PWM_on() {
	TCCR3A = (1 << COM3A0);

	TCCR3B = (1 << WGM32) | (1 << CS31) | (1 << CS30);

	set_PWM(0);
}

void PWM_off() {
	TCCR3A = 0x00;
	TCCR3B = 0x00;
}

enum States{Start, init, inc, dec, wait, toggle} state;

unsigned char button;

double array[9] = {261.63, 293.66, 329.63, 349.23, 392.00, 440.00, 493.88, 523.25, 0};
double temp = 0;
unsigned char temp2 = 0x00;
unsigned char current = 0x00;
unsigned char press = 0x00;

void Tick() {	
	button = (~PINA & 0x07);	
	switch(state) {
		case Start:
			state = init;
			break;
		case init:
			if (button == 0x01) {
				state = inc;
			}
			else if (button == 0x02) {
				state = dec;
			}

			else if (button == 0x04) {
				state = toggle;
			}

			else {
				state = init;
			}
			break;

		case inc:
			temp = array[current];
			state = wait;
			break;

		case dec:
			temp = array[current];
			state = wait;
			break;

			
		case wait:
			if (button == 0x00) {
				state = init;
			}

			else {
				state = wait;
			}
			break;

		case toggle:
			state = wait;
			break;

		default:
			break;
}

	switch(state) {
		case Start:
			break;

		case init:
			break;

		case inc:
			if ((current + 0x01) > 0x07) {
				current = 0x07;
			}

			else {
				current++;
			}
			break;

		case dec:
			if ((current - 0x01) < 0x00) {
				current = 0x00;
			}

			else {
				current--;
			}
			break;

		case wait:
			temp = array[current];
			set_PWM(temp);
			break;

		case toggle:
			if(press) {
				PWM_off();
				temp2 = current;
				current = 0x08;
				press = 0x00;
			}

			else {
				PWM_on();
				current = temp2;
				press = 0x01;
			}
			break;
		default:
		break;
	}
}

int main(void) {
    /* Insert DDR and PORT initializations */
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
	
	state = Start;
	PWM_on();
	press = 0x01;
    /* Insert your solution below */
    while (1){
	    Tick();
    }
    return 1;
}
