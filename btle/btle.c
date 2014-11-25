/*
 * btle.c
 *
 *  Created on: Nov 3, 2014
 *      Author: jcobb
 */

#include <string.h>
#include <stdio.h>
#include <avr/io.h>
#include <stdlib.h>
#include <stdbool.h>
#include <avr/pgmspace.h>
#include "../util/defines.h"
#include "../queue/queue.h"
#include "../usart/usart_btle.h"
#include "btle.h"
#include "btle_msg.h"
#include "btle_driver.h"
#include "../usart/usart_wan.h"

// queue management
queue_t btle_queue;

void btle_init()
{
	//btle_usart_init();
	// set portd bit 5 as output
	DDRD |= _BV(PD5);
	// set portd bit 4 as input
	DDRD &= ~_BV(PD4);

	btle_driver_init();
	btle_set_cts();
}

void btle_set_cts()
{
	//pd5 low
	PORTD &= ~_BV(PD5);
}

uint8_t btle_get_rts()
{
	// return logic high or low
	return (PIND & _BV(PD4));
}

void build_app_msg(btle_msg_t *btle_msg, app_msg_t *msg)
{

	msg->messageType = 1;
	msg->nodeType = 1;
	msg->extAddr = btle_msg->mac;
	msg->shortAddr = 0;
	msg->routerAddr = 1;
	//softVersion;
	//channelMask;
	msg->panId = 0x1976;
	msg->workingChannel = 0x16;
	msg->parentShortAddr = 1;
	msg->lqi = 0;

	msg->rssi = btle_msg->rssi;
	msg->battery = btle_msg->batt;
	msg->temperature = btle_msg->temp;

}

//void build_app_msg(btle_msg_t *btle_msg, app_msg_t *msg)
//{
//msg->message_0 = 'G';
//msg->message_1 = 'I';
//msg->message_2 = 'L';
//}

void btle_tick()
{

	btle_driver_tick();
	// check to see if we have a new message
	if (btle_queue.count > 0)
	{
		app_msg_t app_msg;
		frame_header_t frame_header;
		uint8_t frame[80];

		queue_header_t *qh;
		qh = btle_queue.head;

		btle_msg_t *msg = (btle_msg_t *) QUEUE_DATA(qh);
		build_app_msg(msg, &app_msg);

		// TODO: Handle Messages
		// push out the lw-mesh radio
//		if (!(PINB & (1 << PB0)))
//		{

		frame_header.length = (sizeof(frame_header) - 1) + sizeof(app_msg) + 1;
		frame_header.command = 0x01;
		frame_header.pan_id = 'Z';
		frame_header.short_id = 'Y';
		frame_header.temp_id = 'P';

//		app_msg.message_0 = 'X';
//		app_msg.message_1 = 'X';
//		app_msg.message_2 = 'X';
		int i = 0;
		for (; i < sizeof(frame_header_t); i++)
		{
			frame[i] = ((uint8_t *) (&frame_header))[i];
		}
		int j = 0;
		for (i = 6; j < sizeof(app_msg_t); i++)
		{
			//wan_usart_transmit_string("test");
			frame[i] = ((uint8_t *) (&app_msg))[j];
			j++;

		}

		frame[i+1] = 0xFF;

		wan_usart_transmit_bytes((char*) frame, i + 1);
		//wan_usart_transmit_bytes((char*) sizeof(frame_header), 1);
		//encode_string(&msg);
		//}
		// Dequeue the message
		queue_remove(&btle_queue, (queue_header_t*) msg);

	}
}

queue_results_t btle_enqueue(btle_msg_t *msg)
{
	//queue_results_t result = queue_enqueue(&btle_queue, &msg, sizeof(btle_msg_t));
	queue_results_t result = queue_enqueue(&btle_queue, msg,
			sizeof(btle_msg_t));

	return result;
}

