/**
* \file
*
* \brief The EPD configurations
*
* Copyright (c) 2012-2015 Pervasive Displays Inc. All rights reserved.
*
*  Authors: Pervasive Displays Inc.
*
*  Redistribution and use in source and binary forms, with or without
*  modification, are permitted provided that the following conditions
*  are met:
*
*  1. Redistributions of source code must retain the above copyright
*     notice, this list of conditions and the following disclaimer.
*  2. Redistributions in binary form must reproduce the above copyright
*     notice, this list of conditions and the following disclaimer in
*     the documentation and/or other materials provided with the
*     distribution.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
*  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
*  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
*  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
*  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
*  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
*  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
*  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
*  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
//#include <math.h>
#include <stdio.h>
// Driverlib includes
/* #include "hw_types.h" */
/* #include "hw_ints.h" */
/* #include "hw_memmap.h" */
/* #include "hw_common_reg.h" */
/* #include "interrupt.h" */
/* #include "hw_apps_rcm.h" */
/* #include "prcm.h" */
/* #include "rom.h" */
/* #include "rom_map.h" */
/* #include "prcm.h" */
/* #include "gpio.h" */
/* #include "utils.h" */
/* #include "pin.h" */
/* #include "spi.h" */
/* #include "timer.h" */
// Common interface includes
/* #include "timer_if.h" */
#include "cmsis/nrf52/nrf.h"

#include <if/gpio.h>

#ifndef CONF_EPD_H_INCLUDED
#define CONF_EPD_H_INCLUDED
/**
 * \brief Developer needs to create an external function if wants to read flash */
typedef void (*EPD_read_memory_handler)(long flash_address,uint8_t *target_buffer,
		uint8_t byte_length);

#if !defined(FALSE)
#define FALSE 0 /**< define FALSE=0 */
#endif

#if !defined(TRUE)
#define TRUE (1) /**< define TRUE=1 */
#endif

#if !defined(NULL)
#define NULL (void *)0  /**< define NULL */
#endif

#if !defined(_NOP)
#define _NOP() asm("nop")
#endif

#if !defined(bool)
#define bool uint8_t
#endif

extern void delay_ms(unsigned int ms);

#define EPD_144 0 /**< 1.44 inch PDi EPD */
#define EPD_200 1 /**< 2 inch PDi EPD */
#define EPD_270 2 /**< 2.7 inch PDi EPD */
#define EPD_190 3 /**< 1.9 inch PDi EPD */
#define EPD_260 4 /**< 2.6 inch PDi EPD */



/** Using which EPD size for demonstration.
 * Options are
 * EPD_144 for 1.44 inch
 * EPD_200 for 2.00 inch
 * EPD_270 for 2.70 inch
 * EPD_190 for 1.90 inch
 * EPD_260 for 2.60 inch*/
#define USE_EPD_Type EPD_270


/** \brief Supported COG and FPL type
 * \note
 * - Configure the option here to assign which COG+FPL driving waveform will be used.
 * - Options are G2_Aurora_Ma, G1_Aurora_Mb and G2_Aurora_Mb
 * - How to identify the COG and FPL type of your EPD, visit http://www.pervasivedisplays.com/products/label_info */
#define G2_Aurora_Mb


/** The SPI frequency of this kit  */
#define COG_SPI_baudrate 10000000

int msleep(unsigned int);
int usleep(unsigned int);

#include "EPD_hardware_driver.h"
#include "COG_FPL.h"
#include "EPD_interface.h"
#endif /* CONF_EPD_H_INCLUDED */

