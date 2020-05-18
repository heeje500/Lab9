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

volatile unsigned char TimerFlag = 0;
unsigned long_avr_timer_M = 1;
unsigned long_avr_timer_cntcurr = 0;

void TimerOn() {
	TCCR1B = 0x0B;
	OCR1A = 125;
	TIMSK1 = 0x02;
	TCNT1 = 0;
	_avr_timer_cntcurr = _avr_timer_M;
	SREG |= 0x80;
}

void TimerOff() {
	TCCR1B = 0x00;
}

void TimerISR() {
	TimerFlag = 1;
}

ISR(TIMER1_COMPA_vect) {
	_avr_timer_cntcurr--;
	if(_avr_timer_cntcurr == 0) {
		TimerISR();
		_avr_timer_cntcurr = _avr_timer_M;
	}
}

void TimerSet(unsigned long M) {
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avrtimer_M;
}


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

enum States{Start, init, on, off} state;

unsigned char button;
unsigned char button1;
unsigned char button2;
unsigned char button3;

void Tick() {	
	button = (~PINA & 0x07);
	button1 = (~PINA & 0x01);
	button2 = (~PINA & 0x02);
	button3 = (~PINA & 0x04);

	switch(state) {
		case Start:
			state = init;
			break;
		case init:
			if (button != 0x00) {
				state = on;
			}
			else {
				state = init;
			}
			break;

		case on:
			if (button != 0x00) {
				state = on;
			}
			else {
				state = off;
			}
			break;

		case off:
			state = init;
			break;
			
		default:
			break;
}

	switch(state) {
		case Start:
			break;

		case init:
			set_PWM(0);
			break;

		case on:
			if (button1 == 0x01) {
				set_PWM(261.63);
			}
			
			else if (button2 == 0x02) {
				set_PWM(293.66);
			}

			else if (button3 == 0x04) {
				set_PWM(329.63);
			}
			break;

		case off:
			set_PWM(0);
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
    /* Insert your solution below */
    while (1){
	    Tick();
    }
    return 1;
}
