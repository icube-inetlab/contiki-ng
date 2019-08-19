/*
 * Copyright (c) 2006, Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of the Contiki operating system.
 *
 */

/**
 * \file
 *         A very simple Contiki application showing how Contiki programs look
 * \author
 *         Adam Dunkels <adam@sics.se>
 */

/*
  author : Pacheu Maxime
  This example is designed to work on Platform efr32tb for the Board stghw00036
  This example is supposed to work with the I2C_EFM32_SLAVE written
  in simplicity studio, flashed on the Main MCU
  Or with an XLBee that sets the level of the MOSI pin
  of the SPI bus, and works as an I2C slave with the right address.

  It shows the simple Write and Read transfer operations on I2C
*/
#include "contiki.h"

#include <stdio.h> /* For printf() */
#include <string.h>
#include "dev/i2c.h"
#include "dev/gpio-hal.h"
#include "em_emu.h"

#ifndef I2C1_BUS
#error I2C1_BUS must be set!
#endif /* I2C1_BUS */

#define CUSTOM_ADDR       0x09

#define INT_PIN           6
#define INT_PORT          2

volatile bool temp = false;
volatile process_event_t temp_event;
/*---------------------------------------------------------------------------*/
/*
 * Callback function
 */
static void
event_callback(gpio_hal_port_t port, gpio_hal_pin_mask_t pin_mask)
{
  (void)port;
  (void)pin_mask;
  gpio_hal_arch_toggle_pin(3, 15);
  temp = true;
  //process_post(PROCESS_BROADCAST, temp_event, NULL);
}

/*---------------------------------------------------------------------------*/
PROCESS(hello_world_i2c_process, "Hello world I2C process");
AUTOSTART_PROCESSES(&hello_world_i2c_process);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(hello_world_i2c_process, ev, data)
{
  gpio_hal_arch_toggle_pin(3, 14); //led green

  static gpio_hal_event_handler_t event_handler =
  {
    .next = NULL,
    .handler = event_callback,
    .pin_mask = gpio_hal_pin_to_mask(INT_PIN)
  };
  gpio_hal_pin_cfg_t cfg = GPIO_HAL_PIN_CFG_EDGE_RISING
                          | GPIO_HAL_PIN_CFG_EDGE_RISING
                          | GPIO_HAL_PIN_CFG_INT_ENABLE
                          | GPIO_HAL_PIN_CFG_PULL_UP;

  extern i2c_bus_t I2C1_BUS;
  static i2c_device_t i2c_dev = {
    .bus = &I2C1_BUS,
    .speed = I2C_NORMAL_BUS_SPEED,
    .timeout = 1000,
    .address = CUSTOM_ADDR << 1
  };

  char * msg = "What?";
	uint8_t readBuf[16] 	= {0};
  temp_event = process_alloc_event();

  gpio_hal_register_handler(&event_handler);
  gpio_hal_arch_pin_set_input(INT_PORT,INT_PIN);
  gpio_hal_arch_pin_cfg_set(INT_PORT, INT_PIN, cfg);
  gpio_hal_arch_interrupt_enable(INT_PORT,INT_PIN);

  PROCESS_BEGIN();

  while(1) {

     PROCESS_WAIT_EVENT_UNTIL(temp);
     /*Write n bytes
      */
      if(i2c_acquire(&i2c_dev) == I2C_BUS_STATUS_OK)
      {
        if (i2c_write(&i2c_dev, (uint8_t*)msg, 5) == I2C_BUS_STATUS_OK)
          printf("Custom cmd written\n");
        if(i2c_release(&i2c_dev) != I2C_BUS_STATUS_OK)
        {
          printf("i2c_release pb\n");
          return 1;
        }
      }
      /*Read n bytes
       */
      if(i2c_acquire(&i2c_dev) == I2C_BUS_STATUS_OK)
      {
        if (i2c_read(&i2c_dev, readBuf, 5) == I2C_BUS_STATUS_OK)
          printf("Received: %s\n", (char*)readBuf);
        if(i2c_release(&i2c_dev) != I2C_BUS_STATUS_OK)
        {
          printf("i2c_release pb\n");
          return 1;
        }
      }
      temp = false;
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
