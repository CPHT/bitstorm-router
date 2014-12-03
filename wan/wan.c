/*
 * wan.c
 *
 *  Created on: Nov 5, 2014
 *      Author: titan
 */


#include <string.h>
#include <stdio.h>
#include <avr/io.h>
#include <stdlib.h>
#include <stdbool.h>
#include <avr/pgmspace.h>
#include "../util/defines.h"
#include "../queue/queue.h"
#include "../usart/usart_wan.h"
#include "wan.h"
#include "wan_msg.h"
#include "wan_driver.h"

// queue management
queue_t wan_queue;

void wan_init()
{
	wan_usart_init();
	//wan_usart_get_device_address();

	wan_driver_init();
}

void wan_set_cts()
{
	//pd0 low
	PORTD &= ~_BV(PD0);
}

uint8_t wan_get_rts()
{
	// return logic high or low
	return (PIND & _BV(PD1));
}


void wan_tick()
{
	wan_driver_tick();

	// check to see if we have a new message
	if(wan_queue.count > 0) {

		queue_header_t *qh;
		qh = wan_queue.head;

		wan_msg_t *msg = (wan_msg_t *) QUEUE_DATA(qh);
		// TODO: Handle Messages
		// do something with the message

		// Dequeue the message
		queue_remove(&wan_queue, (queue_header_t*) msg);

	}
}

queue_results_t wan_enqueue(wan_msg_t *msg)
{
	//queue_results_t result = queue_enqueue(&wan_queue, &msg, sizeof(wan_msg_t));
	queue_results_t result = queue_enqueue(&wan_queue, msg, sizeof(wan_msg_t));

	return result;
}
