/**
* \file
*
* \brief The SPI, GPIO, PWM, Temperature definitions of COG hardware driver
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

#ifndef 	DISPLAY_HARDWARE_DRIVCE_H_INCLUDED_
#define 	DISPLAY_HARDWARE_DRIVCE_H_INCLUDED_

#include "conf_EPD.h"

#define	_BV(bit)   				(1 << (bit % 8)) /**< left shift 1 bit */
#define	_HIGH      				1            /**< signal high */
#define	_LOW       				!_HIGH       /**< signal low */


#define	config_gpio_dir_o(PinIO,GPIO_BASE,ucPin) gpio_direction_output(ucPin, 0)
#define	config_gpio_dir_i(PinIO,GPIO_BASE,ucPin) gpio_direction_input(ucPin)

#define	set_gpio_high(GPIO_BASE,Pin)      gpio_set(Pin, 1)    /**< set HIGH for an IOPORT pin */
#define	set_gpio_low(GPIO_BASE,Pin)       gpio_set(Pin, 0)    /**< set LOW for an IOPORT pin */
//#define	set_gpio_invert(GPIO_BASE,Pin)    BITINV(GPIO_BASE,Pin) /**< toggle the value of an IOPORT pin */
#define	input_get(GPIO_BASE,Pin)          gpio_get(Pin)       /**< get current value of an IOPORT pin */

#define Temper_PinIO            PIN_59
#define Temper_PIN              4
#define Temper_PORT             GPIOA0_BASE       /**< LaunchPad P6.6 */

#define SPICLK_PinIO            PIN_05
#define SPICLK_PIN              14
#define SPICLK_PORT             GPIOA1_BASE       /**< LaunchPad P3.2 */

#define EPD_BUSY_PinIO          PIN_62
#define EPD_BUSY_PIN            7
#define EPD_BUSY_PORT           GPIOA0_BASE       /**< LaunchPad P2.7  */

#define PWM_PinIO               PIN_01
#define PWM_PIN                 10
#define PWM_PORT                GPIOA1_BASE       /**< LaunchPad P4.2  */

#define EPD_RST_PinIO           PIN_02
#define EPD_RST_PIN             11
#define EPD_RST_PORT            GPIOA1_BASE       /**< LaunchPad P4.1 */

#define EPD_PANELON_PinIO       PIN_15
#define EPD_PANELON_PIN         22
#define EPD_PANELON_PORT        GPIOA2_BASE       /**< LaunchPad P8.1  */

#define EPD_DISCHARGE_PinIO     PIN_55
#define EPD_DISCHARGE_PIN       1
#define EPD_DISCHARGE_PORT      GPIOA0_BASE       /**< LaunchPad P2.3  */

#define EPD_BORDER_PinIO        PIN_21
#define EPD_BORDER_PIN          25
#define EPD_BORDER_PORT         GPIOA3_BASE       /**< LaunchPad P2.6  */

#define SPIMISO_PinIO           PIN_06
#define SPIMISO_PIN             15
#define SPIMISO_PORT            GPIOA1_BASE       /**< LaunchPad P3.1  */

#define SPIMOSI_PinIO           PIN_07
#define SPIMOSI_PIN             16
#define SPIMOSI_PORT            GPIOA2_BASE       /**< LaunchPad P3.0  */

#define Flash_CS_PinIO          PIN_08
#define Flash_CS_PIN            17
#define Flash_CS_PORT           GPIOA2_BASE       /**< LaunchPad P2.2  */

#define EPD_CS_PinIO            PIN_18
#define EPD_CS_PIN              28
#define EPD_CS_PORT             GPIOA3_BASE       /**< LaunchPad P2.0  */


#define EPD_IsBusy()        (bool)input_get(EPD_BUSY_PORT,EPD_BUSY_PIN)
#define EPD_cs_high()       set_gpio_high(EPD_CS_PORT,EPD_CS_PIN)
#define EPD_cs_low()        set_gpio_low(EPD_CS_PORT,EPD_CS_PIN)
#define EPD_flash_cs_high() set_gpio_high(Flash_CS_PORT,Flash_CS_PIN)
#define EPD_flash_cs_low()  set_gpio_low(Flash_CS_PORT,Flash_CS_PIN)
#define EPD_rst_high()      set_gpio_high(EPD_RST_PORT,EPD_RST_PIN)
#define EPD_rst_low()       set_gpio_low(EPD_RST_PORT,EPD_RST_PIN)
#define EPD_discharge_high()set_gpio_high(EPD_DISCHARGE_PORT,EPD_DISCHARGE_PIN)
#define EPD_discharge_low() set_gpio_low(EPD_DISCHARGE_PORT,EPD_DISCHARGE_PIN)
#define EPD_Vcc_turn_on()   set_gpio_high(EPD_PANELON_PORT,EPD_PANELON_PIN)
#define EPD_Vcc_turn_off()  set_gpio_low(EPD_PANELON_PORT,EPD_PANELON_PIN)
#define EPD_border_high()   set_gpio_high(EPD_BORDER_PORT,EPD_BORDER_PIN)
#define EPD_border_low()    set_gpio_low(EPD_BORDER_PORT,EPD_BORDER_PIN)
#define EPD_pwm_high()      set_gpio_high(PWM_PORT,PWM_PIN)
#define EPD_pwm_low()       set_gpio_low(PWM_PORT,PWM_PIN)
#define SPIMISO_low()       set_gpio_low(SPIMISO_PORT,SPIMISO_PIN)
#define SPIMOSI_low()       set_gpio_low(SPIMOSI_PORT,SPIMOSI_PIN)
#define SPICLK_low()        set_gpio_low(SPICLK_PORT,SPICLK_PIN)
//========================================================================================================


void system_init(void);
void epd_spi_init (void);
void epd_spi_attach (void);
void epd_spi_detach (void);
void epd_spi_send (unsigned char Register, unsigned char *Data, unsigned Length);
void epd_spi_send_byte (uint8_t Register, uint8_t Data);
uint8_t epd_spi_read(unsigned char RDATA);
void epd_spi_write (unsigned char Data);
uint8_t epd_spi_write_ex (unsigned char Data);
void sys_delay_ms(unsigned int ms);
void start_EPD_timer(void);
void stop_EPD_timer(void);
uint32_t get_current_time_tick(void);
void set_current_time_tick(uint32_t count);
void PWM_start_toggle(void);
void PWM_stop_toggle(void);
void PWM_run(uint16_t time);
void initialize_temperature(void);
int16_t get_temperature(void);
void EPD_display_hardware_init (void);
uint8_t SPI_R(uint8_t Register, uint8_t Data);
#endif 	//DISPLAY_HARDWARE_DRIVCE_H_INCLUDED_
