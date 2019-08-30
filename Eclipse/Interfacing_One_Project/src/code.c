#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

unsigned char time[6] = {0}; // array to hold current time


void INT0_Init(void){
	MCUCR |= (1 << ISC01); // generate interrupt on Falling edge of PD2
	GICR |= (1 << INT0); // enable MIE for INT0
}


void INT1_Init(void){
	MCUCR |= (1 << ISC11); // generate interrupt on Falling edge of PD3
	GICR |= (1 << INT1); // enable MIE for INT1
}


void Timer1_CTC_Init(void){

	TCCR1A = (1 << FOC1A); // Channel 1 is non-PWM

	// CTC Mode
	// 1024 Prescaler
	TCCR1B = (1 << WGM12) | (1 << CS10) | (1 << CS12);
	TCNT1 = 0;
	OCR1A = 1000;
	TIMSK |= (1 << OCIE1A);
}

ISR(INT0_vect){
	unsigned char i;
	// reset all numbers
	for(i=0; i<6; i++)
		time[i] = 0;

}
ISR(INT1_vect){
	if(!( TCCR1B & 0x07))
		TCCR1B |= (1 << CS10) | (1 << CS12);
	else
		TCCR1B &= ~0x07;

}
ISR(TIMER1_COMPA_vect){

	// if time is 99:59:59
	if(
			time[5] == 9 && time[4] == 9
			&& time[3] == 5 && time[2] == 9
			&& time[1] == 5 && time[0] == 9){

		//reset clock
		time[5] = time[4] = time[3] = time[2] = time[1] = time[0] = 0;
	}

	// if time is x9:59:59

	else if(
			time[4] == 9 && time[3] == 5 && time[2] == 9
			&& time[1] == 5 && time[0] == 9){

		//increase second digit of hours
		time[4] = time[3] = time[2] = time[1] = time[0] = 0;
		time[5]++ ;

	}

	// if time is xx:59:59
	else if(time[3] == 5 && time[2] == 9 && time[1] == 5 && time[0] == 9){

		//increase first digit of hours
		time[3] = time[2] = time[1] = time[0] = 0;
		time[4]++ ;

	}
	// if time is xx:x9:59
	else if(time[2] == 9 && time[1] == 5 && time[0] == 9){

		//increase second digit of minutes
		time[2] = time[1] = time[0] = 0;
		time[3]++;
	}

	//if time is xx:xx:59
	else if(time[1] == 5 && time[0] == 9){

		// increase first digit of minutes
		time[2]++;
		time[1] = time[0] = 0;
	}

	// if time is xx:xx:x9
	else if(time[0] == 9){

		//increase second digit of seconds
		time[0] = 0;
		time[1]++;
	}
	else{

		//increase first digit of seconds
		time[0]++;
	}
}
int main(){
	unsigned char i;
	DDRC |= 0x0F;		// 7 seg is output
	PORTC &= ~ 0x0F;	// display 0 on 7 seg
	DDRB |= 0x3F;		// control 7 segments
	PORTB &= ~0x3F;		// make all 7 seg OFF
	DDRD &= (1 << PD2);   // make reset button input
	PORTD |= (1 << PD2);  // enable internal pull up for PD2
	DDRD &= (1 << PD3);		// make stop button input
	PORTD |= (1 << PD3);	// enable internal pull up for PD3
	Timer1_CTC_Init();	// initialize timer1 in CTC mode
	INT0_Init();		// initialize INT0
	INT1_Init();		// initialize INT1
	SREG |= (1 << 7);	// enable global interrupt

	while(1){

		if(i == 6)
			i = 0;

		// make 7 seg number i ON and all another 7 seg OFF
		PORTB = (1 << i);
		// display the number of 7 seg number i
		PORTC = (PORTC & 0xF0) | (time[i] & 0x0F);
		i++;
		_delay_ms(1);



	}
}
