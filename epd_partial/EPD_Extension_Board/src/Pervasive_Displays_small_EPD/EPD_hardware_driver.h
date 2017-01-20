/**
* \file
*
* \brief The SPI, PWM, Temperature definitions of COG hardware driver
*
* Copyright (c) 2012-2014 Pervasive Displays Inc. All rights reserved.
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

#define	_BV(bit)   (1 << (bit)) /**< left shift 1 bit */
#define	_HIGH      1            /**< signal high */
#define	_LOW       !_HIGH       /**< signal low */

//FOR MSP430
#define DIR_(x) 			    x ## DIR
#define _DIR(x)				    (DIR_(x))
#define OUT_(x) 			    x ## OUT
#define OUTPORT(x) 			    (OUT_(x))
#define SEL_(x)				    x ## SEL
#define SEL(x)				    (SEL_(x))
#define SEL2_(x)			    x ## SEL2
#define SEL2(x)				    (SEL2_(x))
#define IN_(x) 				    x ## IN
#define INPORT(x) 			    (IN_(x))
#define REN_(x) 			    x ## REN
#define REN(x) 				    (REN_(x))
#define BITSET(x,y) 		    ((x) |= (y))
#define BITCLR(x,y) 		    ((x) &= ~(y))
#define BITINV(x,y) 		    ((x) ^= (y))

#define	config_gpio_dir_o(Port,Pin)  BITSET(_DIR (Port), Pin) /**< set output direction for an IOPORT pin */
#define	config_gpio_dir_i(Port,Pin)  BITCLR(_DIR (Port), Pin)  /**< set input direction for an IOPORT pin */
#define	set_gpio_high(Port,Pin)      BITSET (OUTPORT (Port), Pin) /**< set HIGH for an IOPORT pin */
#define	set_gpio_low(Port,Pin)       BITCLR (OUTPORT (Port), Pin)  /**< set LOW for an IOPORT pin */
#define	set_gpio_invert(Port,Pin)    BITINV(OUTPORT (Port),Pin) /**< toggle the value of an IOPORT pin */
#define	input_get(Port,Pin)          (INPORT (Port)  & Pin )   /**< get current value of an IOPORT pin */

#if (defined __MSP430F5529__)
#define Temper_PIN              BIT6
#define Temper_PORT             P6       /**< LaunchPad P6.6 */
#define	Temper_PORT_SEL	       (SEL (Temper_PORT))
#define SPICLK_PIN              BIT2
#define SPICLK_PORT             P3       /**< LaunchPad P3.2 */
#define EPD_BUSY_PIN            BIT7
#define EPD_BUSY_PORT           P2       /**< LaunchPad P2.7  */
#define PWM_PIN                 BIT2
#define PWM_PORT                P4       /**< LaunchPad P4.2  */
#define EPD_RST_PIN             BIT1
#define EPD_RST_PORT            P4       /**< LaunchPad P4.1 */
#define EPD_PANELON_PIN         BIT1
#define EPD_PANELON_PORT        P8       /**< LaunchPad P8.1  */
#define EPD_DISCHARGE_PIN       BIT3
#define EPD_DISCHARGE_PORT      P2       /**< LaunchPad P2.3  */
#define EPD_BORDER_PIN          BIT6
#define EPD_BORDER_PORT         P2       /**< LaunchPad P2.6  */
#define SPIMISO_PIN             BIT1
#define SPIMISO_PORT            P3       /**< LaunchPad P3.1  */
#define SPIMOSI_PIN             BIT0
#define SPIMOSI_PORT            P3       /**< LaunchPad P3.0  */
#define Flash_CS_PIN            BIT2
#define Flash_CS_PORT           P2       /**< LaunchPad P2.2  */
#define	Flash_CS_PORT_SEL	    (SEL (Flash_CS_PORT))
#define	Flash_CS_PORT_SEL2		(SEL2(Flash_CS_PORT))
#define EPD_CS_PIN              BIT0
#define EPD_CS_PORT             P2       /**< LaunchPad P2.0  */
#define	CS_PORT_SEL		        (SEL (EPD_CS_PORT))
#define	CS_PORT_SEL2		    (SEL2(EPD_CS_PORT))

#elif(defined __MSP430G2553__)

#define Temper_PIN              BIT4
#define Temper_PORT             P1       /**< LaunchPad P1.4 */
#define SPICLK_PIN              BIT5
#define SPICLK_PORT             P1       /**< LaunchPad P1.5 */
#define EPD_BUSY_PIN            BIT0
#define EPD_BUSY_PORT           P2       /**< LaunchPad P2.0  */
#define PWM_PIN                 BIT1
#define PWM_PORT                P2       /**< LaunchPad P2.1  */
#define EPD_RST_PIN             BIT2
#define EPD_RST_PORT            P2       /**< LaunchPad P2.2 */
#define EPD_PANELON_PIN         BIT3
#define EPD_PANELON_PORT        P2       /**< LaunchPad P2.3  */
#define EPD_DISCHARGE_PIN       BIT4
#define EPD_DISCHARGE_PORT      P2       /**< LaunchPad P2.4  */
#define EPD_BORDER_PIN          BIT5
#define EPD_BORDER_PORT         P2       /**< LaunchPad P2.5  */
#define SPIMISO_PIN             BIT6
#define SPIMISO_PORT            P1       /**< LaunchPad P1.6  */
#define SPIMOSI_PIN             BIT7
#define SPIMOSI_PORT            P1       /**< LaunchPad P1.7  */
#define Flash_CS_PIN            BIT7
#define Flash_CS_PORT           P2       /**< LaunchPad P2.7  */
#define	Flash_CS_PORT_SEL	    (SEL (Flash_CS_PORT))
#define	Flash_CS_PORT_SEL2		(SEL2(Flash_CS_PORT))
#define EPD_CS_PIN              BIT6
#define EPD_CS_PORT             P2       /**< LaunchPad P2.6  */
#define	CS_PORT_SEL		        (SEL (EPD_CS_PORT))
#define	CS_PORT_SEL2		    (SEL2(EPD_CS_PORT))

#endif
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
#define TACCTL2		        TA0CCTL2
#define TACCTL		        TA0CTL
#define TAR			        TA0R
#define TACCR0		        TA0CCR0

#if (defined __MSP430F5529__)
/**SPI Defines ****************************************************************/
#define SMCLK_FREQ			(25000000)
#define SPISEL              P3SEL
#define SPICTL0				UCB0CTL0
#define SPICTL1				UCB0CTL1
#define SPIBR0				UCB0BR0
#define SPIBR1				UCB0BR1
#define SPIIFG				UCB0IFG
#define SPIRXBUF			UCB0RXBUF
#define SPIRXIFG			UCB0RXIFG
#define SPITXBUF			UCB0TXBUF
#define SPITXIFG			UCTXIFG
#define SPISTAT				UCB0STAT
#define SPIBUSY             UCBUSY
#define SPI_baudrate        (SMCLK_FREQ/COG_SPI_baudrate)           /**< the baud rate of SPI */
/**ADC Defines ****************************************************************/
#define ADCCTL0         ADC12CTL0
#define ADCENC          ADC12ENC
#define ADCSC           ADC12SC
#define ADCIFG          ADC12IFG
#define ADCMEM          ADC12MEM0
#elif(defined __MSP430G2553__)
/**SPI Defines ****************************************************************/
#define SMCLK_FREQ			(16000000) 
#define SPISEL              P1SEL
#define SPISEL2             P1SEL2
#define SPIIE			    IE2
#define SPICTL0				UCB0CTL0
#define SPICTL1				UCB0CTL1
#define SPIBR0				UCB0BR0
#define SPIBR1				UCB0BR1
#define SPIMCTL			    UCA0MCTL
#define SPIIFG				IFG2
#define SPIRXBUF			UCB0RXBUF
#define SPIRXIFG			UCB0RXIFG
#define SPITXBUF			UCB0TXBUF
#define SPITXIFG			UCB0TXIFG
#define SPISTAT				UCB0STAT
#define SPIBUSY             UCBUSY
#define SPI_baudrate        (SMCLK_FREQ/COG_SPI_baudrate)
/**ADC Defines ****************************************************************/
#define ADCCTL0         	ADC10CTL0
#define ADCENC          	ENC
#define ADCSC           	ADC10SC
#define ADCIFG          	ADC10IFG
#define ADCMEM          	(long)ADC10MEM
#endif

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
