
#define BAUD_RATE 38400
#include "BasicSerial3INT.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define HIGH 1
#define LOW  0

#define LRED_DIR  DDRB  |=  _BV(PB3)
#define LRED_HIGH PORTB |=  _BV(PB3)
#define LRED_LOW  PORTB &=~ _BV(PB3)
#define LRED_INV  PORTB ^=  _BV(PB3)

#define LGREEN_DIR  DDRB  |=  _BV(PB4)
#define LGREEN_HIGH PORTB |=  _BV(PB4)
#define LGREEN_LOW  PORTB &=~ _BV(PB4)
#define LGREEN_INV  PORTB ^=  _BV(PB4)

#define BTN1_DIR    DDRB  &=~ _BV(PB2)
#define BTN1_PULLUP PORTB |=  _BV(PB2)
#define BTN1_IN     PINB & _BV(PB2)

#define BTN2_DIR    DDRB  &=~ _BV(PB0)
#define BTN2_PULLUP PORTB |=  _BV(PB0)
#define BTN2_IN     PINB & _BV(PB0)

void serOut(const char* str) {
	while (*str) TxByte (*str++);
}

ISR(INT0_vect,ISR_BLOCK) {

	unsigned char c = RxByte();
	serOut("Serial: ");
	TxByte(c);
	serOut("\r\n");

	switch(c) {
		case '1':
			LGREEN_HIGH;
			break;
		case '2':
			LGREEN_LOW;
			break;
		case '3':
			LRED_HIGH;
			break;
		case '4':
			LRED_LOW;
			break;
	}

}
 
int main() {
	LRED_DIR;
	LRED_LOW;

	LGREEN_DIR;
	LGREEN_LOW;

	BTN1_DIR;
	BTN1_PULLUP;

	BTN2_DIR;
	BTN2_PULLUP;

	MCUCR |=  _BV(ISC01);
	MCUCR &=~ _BV(ISC00);
	GIMSK = 0b01000000; // INT0のみ割り込み

	sei();

	unsigned char btn1 = HIGH;
	unsigned char btn1_prev = HIGH;
	unsigned char btn2 = HIGH;
	unsigned char btn2_prev = HIGH;
	unsigned char button = 0;

	while(1) {

		button=0;

		btn1= BTN1_IN ? HIGH : LOW;
		if( ( btn1 == LOW ) && ( btn1_prev == HIGH )) {
			LRED_HIGH;
			LGREEN_LOW;
			button=1;
		}
		btn1_prev=btn1;

		btn2= BTN2_IN ? HIGH : LOW;
		if( ( btn2 == LOW ) && ( btn2_prev == HIGH )) {
			LGREEN_HIGH;
			LRED_LOW;
			button=2;
		}
		btn2_prev=btn2;

		if( button ) {
			char buf[]="Button:  \r\n";
			buf[8]= ( button+48 ); // 数値を文字列に変換

			cli();
			serOut(buf);
			sei();

		}

	}

	return 0;
}

