#include "contiki-conf.h"
#include "dev/slip.h"
#include "debug-uart.h"


void slip_arch_writeb(unsigned char c)
{
  dbg_putchar((unsigned char)c);
}


void slip_arch_init(void)
{
  debug_uart_set_input_handler(slip_input_byte);
}
