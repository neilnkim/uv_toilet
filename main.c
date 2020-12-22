/**************************************************
* Project
*
* Copyright
* All Rights Reserved by ~
*
* written by Neil
*
**************************************************/

#include "main.h"

//----------------------------------------------------------------------------

#include <avr/interrupt.h>

volatile unsigned int timer_overflow_count = 0;
unsigned char g_state = STATE_IDLE;
unsigned char g_key_pressed = 0;
unsigned char g_cover_state = _OPEN_;
unsigned char g_mode = MODE_5MIN;
unsigned char led_count_10ms=0;

ISR(TIM0_OVF_vect) {
	timer_overflow_count++;
}

void init_timer(void)
{
	// prescale timer to 1/1024th the clock rate
	TCCR0B |= (1<<CS02) | (1<<CS00);

	// enable timer overflow interrupt
	TIMSK0 |=1<<TOIE0;
}

/*
	B0: input;		function select;	L=button press, H=button release
	B1: output; 	green led;			L=turn on, H=turn off
	B2: output; 	red led;			L=turn on, H=turn off
	B3: output; 	uvc led;			L=turn off, H=turn on
	B4: input; 		tilt switch;		L=open, H=closed
	B5: input; 		RESET
*/
void init_gpio(void)
{
	DDRB = 0b00001110;
	PORTB= 0b00000000;
}

#if 0
void delay_10us(unsigned short time_ms)
{
	register unsigned short i, j;
	for(j=0;j<10;j++)
	{
		for(i=0; i<time_ms; i++)
		{
			asm volatile(" PUSH  R0 ");
			asm volatile(" POP   R0 ");
		}
	}
}
#endif

void delay_1ms(unsigned short time_ms)
{
	register unsigned short i, j;
	for(j=0;j<50;j++)
	{
		for(i=0; i<time_ms; i++)
		{
			asm volatile(" PUSH  R0 ");
			asm volatile(" POP   R0 ");
		}
	}
}

void set_green_led(int st)
{
	// H=off, L=on
	if(st)
	{
		PORTB &= ~BIT1;
	}
	else
	{
		PORTB |= BIT1;
	}
}

void set_red_led(unsigned char st)
{
	// H=off, L=on
	if(st)
	{
		PORTB &= ~BIT2;
	}
	else
	{
		PORTB |= BIT2;
	}
}

void set_uvc_led(unsigned char st)
{
	// H=on, L=off
	if(st)
	{
		PORTB |= BIT3;
	}
	else
	{
		PORTB &= ~BIT3;
	}
}

// B0: H = released, L = pressed
void check_function_key_pressed(void)
{
	static unsigned char key_cnt_10ms=0;

	if(PINB&BIT0) // HIGH
	{
		g_key_pressed = _FALSE_;
		key_cnt_10ms = 0;
	}
	else if(key_cnt_10ms++==10)
		g_key_pressed = _TRUE_;
}

// B4: H=open, L=closed
void check_cover_state(void)
{
	static unsigned char open_cnt_10ms=0, closed_cnt_10ms=0;

	if(PINB&BIT0) // HIGH
	{
		open_cnt_10ms++;
		closed_cnt_10ms=0;
		if(open_cnt_10ms>10)
		{
			open_cnt_10ms = 10;
			g_cover_state = _OPEN_;
		}
	}
	else // LOW
	{
		open_cnt_10ms=0;
		closed_cnt_10ms++;
		if(closed_cnt_10ms>10)
		{
			closed_cnt_10ms = 10;
			g_cover_state = _CLOSED_;
		}
	}
}

void led_player_5min(void)
{
	set_green_led(_ON_);
	set_red_led(_OFF_);
}

void led_player_10min(void)
{
	set_green_led(_OFF_);

	if(30<=led_count_10ms)
	{
		set_red_led(_OFF_);
	}
	else
	{
		set_red_led(_ON_);
	}

/*
	if(0<=led_count_10ms && led_count_10ms<30)
	{
		set_red_led(_ON_);
	}
	else if(30<=led_count_10ms && led_count_10ms<200)
	{
		set_red_led(_OFF_);
	}
*/
	led_count_10ms++;
	led_count_10ms %= 200;
}

void led_player_15min(void)
{
	set_green_led(_OFF_);

	if(90<=led_count_10ms)
	{
		set_red_led(_OFF_);
	}
	else if(60<=led_count_10ms)
	{
		set_red_led(_ON_);
	}
	else if(30<=led_count_10ms)
	{
		set_red_led(_OFF_);
	}
	else
	{
		set_red_led(_ON_);
	}
/*
	if(0<=led_count_10ms && led_count_10ms<30)
	{
		set_red_led(_ON_);
	}
	else if(30<=led_count_10ms && led_count_10ms<60)
	{
		set_red_led(_OFF_);
	}
	else if(60<=led_count_10ms && led_count_10ms<90)
	{
		set_red_led(_ON_);
	}
	else if(90<=led_count_10ms && led_count_10ms<200)
	{
		set_red_led(_OFF_);
	}
*/
	led_count_10ms++;
	led_count_10ms %= 200;
}

int main(void)
{
	unsigned int state_timeout;

	init_gpio();
	init_timer();
	sei();

	g_state = STATE_IDLE;
	set_green_led(_OFF_);
	set_red_led(_OFF_);
	set_uvc_led(_OFF_);

	while(1)
	{
		delay_1ms(10);
		check_function_key_pressed();
		check_cover_state();

		switch(g_state)
		{
			case STATE_OPEN:
			{
				if(g_cover_state ==_CLOSED_)
				{
					timer_overflow_count = 0;
					g_state = STATE_CLOSED1;
					break;
				}

				if(g_key_pressed)
				{
					g_key_pressed = 0;
					g_mode++;
					g_mode %= MAX_MODE_NUM;
				}
			}
			break;

			case STATE_CLOSED1:
			{
				set_uvc_led(_OFF_);

				if(timer_overflow_count > SYSTICK_5SEC) //5 sec
				{
					set_uvc_led(_ON_);
					timer_overflow_count = 0;
					g_state = STATE_CLOSED2;
				}
			}
			break;

			case STATE_CLOSED2:
			{
				set_uvc_led(_ON_);

				if(g_mode==MODE_10MIN)
					state_timeout = SYSTICK_10MIN;
				else if(g_mode==MODE_15MIN)
					state_timeout = SYSTICK_15MIN;
				else
					state_timeout = SYSTICK_5MIN;

				if(timer_overflow_count > state_timeout) //5 sec
				{
					set_uvc_led(_OFF_);
					timer_overflow_count = 0;
					g_state = STATE_IDLE;
				}
			}
			break;

			case STATE_IDLE:
			default:
			{
				set_green_led(_OFF_);
				set_red_led(_OFF_);
				set_uvc_led(_OFF_);
				if(g_cover_state ==_OPEN_)
				{
					timer_overflow_count = 0;
					g_state = STATE_OPEN;
				}
			}
			break;
		}

		//update status led
		if(g_state==STATE_IDLE)
		{
			set_green_led(_OFF_);
			set_red_led(_OFF_);
			set_uvc_led(_OFF_);
		}
		else
		{
			switch(g_mode)
			{
				default:
				case MODE_5MIN: 	led_player_5min(); 	break;
				case MODE_10MIN:	led_player_10min();	break;
				case MODE_15MIN:	led_player_15min();	break;
			}
		}
	}

	return 0;
}


