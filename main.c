/*


 * main.c
 *
 *  Created on: Nov 3, 2014
 *      Author: jcobb
 */

#define F_CPU		8000000

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util/clock.h"
#include "util/log.h"
#include "util/config.h"
#include "queue/queue.h"
#include "btle/btle.h"
#include "usart/usart_btle.h"
#include "wan/wan_driver.h"
#include "wan/wan.h"
#include "usart/usart_wan.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>



static const char _tag[] PROGMEM = "main: ";
volatile char term_in = 0;

// timeout helper
volatile clock_time_t future = 0;
bool timeout();
void set_timer(clock_time_t timeout);

void terminal_in_cb(uint8_t c)
{
	term_in = c;

}

void main()
{
	// Blinky test!
	DDRD |= _BV(PD6); // data direction bit
	DDRD |= _BV(PD7);

	DDRB &= _BV(PB0); // WAN_INT_01

	//debug_init(terminal_in_cb);
	btle_usart_init_cb(terminal_in_cb);

	clock_init();
	config_init();

	/*
	 * load configuration
	 */
	//LOG("config_init...\r\n");

	// btle is on usart0
	//btle_init();

	// wan is on usart1
	wan_init();

	sei();

	//wan_usart_transmit_string("Bitstorm router started\r\n");
	btle_usart_transmit_string("Hello BTLE\r\n");

	while(true){
		//btle_tick();

		if(term_in > 0)
		{
			wan_usart_transmit((char*)term_in);
		}

		term_in = 0;

		if(timeout()){
			// just exercising timer
			set_timer(1000);
			PORTD ^= _BV(PD6);


			//wan_usart_transmit_string("Hello WAN\r\n");
			//btle_usart_transmit_string("Hello BTLE\r\n");
			//_delay_ms(1000);
		}

	}
}

void set_timer(clock_time_t timeout)
{
	future = clock_time() + timeout;
}

// timeout routine to demonstrate clock_time
// being kept by pwm isr interrupt
bool timeout()
{
	bool timeout = false;

	if(clock_time() >= future)
	{
		set_timer(1000);
		timeout = true;

	}

	return timeout;
}



