#include <kernel.h>
#include <kdata.h>
#include <printf.h>
#include <stdbool.h>
#include <devtty.h>
#include <device.h>
#include <tty.h>

volatile uint8_t *uart_data = (volatile uint8_t *)0xF03000;	/* UART data */
volatile uint8_t *uart_status = (volatile uint8_t *)0xF03010;	/* UART status */

unsigned char tbuf1[TTYSIZ];

struct s_queue ttyinq[NUM_DEV_TTY + 1] = {	/* ttyinq[0] is never used */
	{NULL, NULL, NULL, 0, 0, 0},
	{tbuf1, tbuf1, tbuf1, TTYSIZ, 0, TTYSIZ / 2},
};

/* Output for the system console (kprintf etc) */
void kputchar(char c)
{
	if (c == '\n')
		tty_putc(1, '\r');
	tty_putc(1, c);
}

ttyready_t tty_writeready(uint8_t minor)
{
	uint8_t c = *uart_status;
	return (c & 2) ? TTY_READY_NOW : TTY_READY_SOON; /* TX DATA empty */
}

void tty_putc(uint8_t minor, unsigned char c)
{
	*uart_data = c;	/* Data */
}

void tty_setup(uint8_t minor)
{
}

int tty_carrier(uint8_t minor)
{
	return 1;
}

void tty_sleeping(uint8_t minor)
{
}

/* Currently run off the timer */
void tty_interrupt(void)
{
	uint8_t r = *uart_status;
	if (r & 1) {
		r = *uart_data;
		tty_inproc(1,r);
	}	
}

void platform_interrupt(void)
{
	timer_interrupt();
	tty_interrupt();
}
