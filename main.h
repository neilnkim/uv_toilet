/**************************************************
* Project
*
* Copyright
* All Rights Reserved by
*
* written by Neil
*
**************************************************/

#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <avr/wdt.h>
#include <string.h>
#include <ctype.h>
#include <avr/pgmspace.h>
#include <avr/sleep.h>
#include "version.h"

// 1tick = 0.55sec
#define SYSTICK_5SEC 	9
#define SYSTICK_5MIN 	(545)
#define SYSTICK_10MIN 	(1090)
#define SYSTICK_15MIN 	(1636)

#define STATE_OPEN 		0
#define STATE_CLOSED1 	1
#define STATE_CLOSED2 	2
#define STATE_IDLE 		3

enum{
	MODE_5MIN  = 0,
	MODE_10MIN,
	MODE_15MIN,
	MAX_MODE_NUM
};

#define sbi(port, bit) 	(port) |=  (1<<(bit))
#define cbi(port, bit) 	(port) &= ~(1<<(bit))

#define BIT(x) 	(1<<(x))
#define BIT0 		0x01
#define BIT1 		0x02
#define BIT2 		0x04
#define BIT3 		0x08
#define BIT4 		0x10
#define BIT5 		0x20
#define BIT6 		0x40
#define BIT7 		0x80
#define ZERO(x) 	0

#define _ON_  		1
#define _OFF_ 		0
#define _TRUE_  	1
#define _FALSE_ 	0
#define _OUT_ 		1
#define _IN_  		0
#define _HIGH_ 		1
#define _LOW_  		0
#define _OPEN_		0
#define _CLOSED_	1


/*******************************************************************
	function prototypes
*******************************************************************/

#endif


