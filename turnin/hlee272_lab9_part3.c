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
#include <avr/interrupt.h> 
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

volatile unsigned char TimerFlag = 0;
unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;

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
	_avr_timer_cntcurr = _avr_timer_M;
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

enum States{Start, init, play, pause, down, wait} state;
double notes[36] = {261.63, 200.18, 271.4, 200.18, 493.8, 200.18, 535.28, 493.8, 349.23, 329.63, 261.63, 261.63, 493.8, 493.8, 200.18, 261.63, 329.63, 349.23, 493.8, 349.23, 200.18, 200.18, 261.63, 271.4, 493.8, 200.18, 329.63, 349.23, 349.23, 329.63, 200.18, 261.63, 271.4, 200.18, 493.8, 200.18} ;
char time[36] = {10, 50, 50, 25, 50, 50, 50, 50 ,10, 50 ,10, 50, 26, 50, 10, 10, 10, 10, 10, 26, 50, 25, 25, 25, 50, 10, 25, 50, 10, 25, 10, 10, 25, 50, 25};
unsigned char i = 0x00;
unsigned char j = 0x00;
unsigned char k = 0x00;
unsigned char button;
unsigned char button1;
unsigned char button2;
unsigned char button3;
double temp = 0;
unsigned char current = 0x00;
unsigned char press = 0x00;
unsigned char temp2 = 0x00;

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
			if (button1 == 0x01) {
				state = play;
			}
			else {
				state = init;
			}
			break;

		case play:
			if (i <= time[j]) {
				state = play;
			}
			else if (i > time[j]) {
				++k;
				state = down;
			}
			break;

		case down:
			if (k < 20) {
				++j;
				state = play;
			}
			else if (k >= 20) {
				state = pause;
			}
			break;

		case pause:
			if (button == 0x01) {
				state = wait;
			}
			else {
				state = pause;
			}
			break;

		case wait:
			if (button1 == 0x01) {
				state = wait;
			}
			else {
				state = init;
			}
			break;

			
		default:
			break;
}

	switch(state) {
		case Start:
			break;

		case init:
			i = 0;
			j = 0;
			k = 0;
			break;

		case play:
			set_PWM(notes[j]);
			i++;
			break;

		case down:
			set_PWM(0);
			break;

		case pause:
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
	TimerSet(100);
	TimerOn();

    /* Insert your solution below */
    while (1){
	    Tick();
	    while(!TimerFlag){}
	    TimerFlag = 0;
    }
    return 1;
}
