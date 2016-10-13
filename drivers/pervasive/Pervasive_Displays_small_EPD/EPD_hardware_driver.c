/**
 * \file
 *
 * \brief The initialization and configuration of COG hardware driver
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


#include "conf_EPD.h"

static uint16_t EPD_Counter;
void TimerBaseIntHandler(void);
/**
 * \brief Set up EPD Timer for 1 mSec interrupts
 *
 * \note
 * desired value: 1mSec
 * actual value:  1.000mSec
 */
 void initialize_EPD_timer(void) {
	 NRF_TIMER4->MODE = TIMER_MODE_MODE_Timer << TIMER_MODE_MODE_Pos;
	 NRF_TIMER4->INTENSET =
		 TIMER_INTENSET_COMPARE0_Set << TIMER_INTENSET_COMPARE0_Pos;
	 NRF_TIMER4->SHORTS = (TIMER_SHORTS_COMPARE0_CLEAR_Enabled << TIMER_SHORTS_COMPARE0_CLEAR_Pos);
	 NRF_TIMER4->CC[0] = 1000;
	 NRF_TIMER4->BITMODE = TIMER_BITMODE_BITMODE_32Bit << TIMER_BITMODE_BITMODE_Pos;
	 NRF_TIMER4->PRESCALER = 4 << TIMER_PRESCALER_PRESCALER_Pos;
	 /* Timer_IF_Init(PRCM_TIMERA0, TIMERA0_BASE, TIMER_CFG_PERIODIC, TIMER_A, 0); */
	 /*  Timer_IF_IntSetup(TIMERA0_BASE, TIMER_A, TimerBaseIntHandler); */
}

/**
 * \brief Start Timer
 */
void start_EPD_timer(void) {
	NRF_TIMER4->TASKS_START = 1;
    /* Timer_IF_Start(TIMERA0_BASE, TIMER_A, */
    /*               PERIODIC_TEST_CYCLES / 1000); */
	EPD_Counter = 0;
}

/**
 * \brief Stop Timer
 */
void stop_EPD_timer(void) {
	/* Timer_IF_Stop(TIMERA0_BASE, TIMER_A); */
	NRF_TIMER4->TASKS_STOP = 1;
	NRF_TIMER4->TASKS_CLEAR = 1;
}

/**
 * \brief Get current Timer after starting a new one
 */
uint32_t get_current_time_tick(void) {
	return EPD_Counter;
}
/**
 * \brief Set current Timer after starting a new one
 */
void set_current_time_tick(uint32_t count) {
     EPD_Counter=count;
}
/**
 * \brief Interrupt Service Routine for system tick counter
 */
void TimerBaseIntHandler(void)
{
	//Timer_IF_InterruptClear(TIMERA0_BASE);
    NRF_TIMER4->EVENTS_COMPARE[0] = 0;
    EPD_Counter++;
}




/**
 * \brief Delay mini-seconds
 * \param ms The number of mini-seconds
 */
void delay_ms(unsigned int ms) {
	/* #define MSEC_VAL 15000 */
	/* MAP_UtilsDelay(MSEC_VAL*ms); */
	msleep(ms);
}

 void Wait_10us(void) {
	//UtilsDelay(1);

	 //FIXME: Specs says CS low-high-low should be greater-equal 80ns
	 usleep(10);
}

//******************************************************************
//* PWM  Configuration/Control //PWM output : PD3
//******************************************************************

/* /\** */
/*  * \brief The PWM signal starts toggling */
/*  *\/ */
/* void PWM_start_toggle(void) { */

/* } */

/* /\** */
/*  * \brief The PWM signal stops toggling. */
/*  *\/ */
/* void PWM_stop_toggle(void) { */

/* } */

/* /\** */
/*  * \brief PWM toggling. */
/*  * */
/*  * \param ms The interval of PWM toggling (mini seconds) */
/*  *\/ */
/* void PWM_run(uint16_t ms) { */
/* 	start_EPD_timer(); */
/* 	do { */
/* 		EPD_pwm_high(); */
/* 		UtilsDelay(30); */
/* 		EPD_pwm_low(); */
/* 		UtilsDelay(30); */
/* 	} while (get_current_time_tick() < ms); //wait Delay Time */
/* 	stop_EPD_timer(); */
/* } */

//******************************************************************
//* SPI  Configuration
//******************************************************************
/**
 * \brief Configure SPI
 */
void epd_spi_init(void) {
	
    /* MAP_PRCMPeripheralClkEnable(PRCM_GSPI, PRCM_RUN_MODE_CLK);   */
}

/**
 * \brief Initialize SPI
 */
void epd_spi_attach(void) {
    /* // MAP_PRCMPeripheralReset(PRCM_SSPI); */
    /* // Configure PIN_05 for SPI0 GSPI_CLK */
    /* MAP_PinTypeSPI(PIN_05, PIN_MODE_7); */
    /* // Configure PIN_06 for SPI0 GSPI_MISO */
    /* MAP_PinTypeSPI(PIN_06, PIN_MODE_7); */
    /* // Configure PIN_07 for SPI0 GSPI_MOSI */
    /* MAP_PinTypeSPI(PIN_07, PIN_MODE_7); */
    /* // Reset SPI */
    /* MAP_SPIReset(GSPI_BASE); */
    /* // Configure SPI interface */
    /* MAP_SPIConfigSetExpClk(GSPI_BASE,MAP_PRCMPeripheralClockGet(PRCM_GSPI), */
    /*                  COG_SPI_baudrate,SPI_MODE_MASTER,SPI_SUB_MODE_0, */
    /*                  (SPI_SW_CTRL_CS | */
    /*                  SPI_4PIN_MODE | */
    /*                  SPI_TURBO_OFF  | */
    /*                  SPI_CS_ACTIVELOW  | */
    /*                  SPI_WL_8)); */
    /* // Enable SPI for communication */
    /* MAP_SPIEnable(GSPI_BASE); */
    /* MAP_SPICSEnable(GSPI_BASE); */

}

/**
 * \brief Disable SPI and change to GPIO
 */
void epd_spi_detach(void) {

	/* MAP_SPICSDisable(GSPI_BASE); */
	/* MAP_SPIDisable(GSPI_BASE); */
	/* config_gpio_dir_o(SPICLK_PinIO,SPICLK_PORT,SPICLK_PIN); */
	/* config_gpio_dir_o(SPIMISO_PinIO,SPIMISO_PORT,SPIMISO_PIN); */
	/* config_gpio_dir_o(SPIMOSI_PinIO,SPIMOSI_PORT,SPIMOSI_PIN); */
	/* SPIMISO_low(); */
	/* SPIMOSI_low(); */
	/* SPICLK_low(); */

}

/**
 * \brief SPI synchronous read
 */
void spi_transaction(const void *tx_buf, size_t tx_len, void *rx_buf,
		size_t rx_len);
uint8_t epd_spi_read(unsigned char data) {

	unsigned char rxData;
	/* MAP_SPITransfer(GSPI_BASE, &data, &rxData, 1, 0); */
	spi_transaction(&data, 1, &rxData, 1); // spi_transfer(..., ORC) add over-reading character to prototype
	return rxData;

}

/**
 * \brief SPI synchronous write
 */
void epd_spi_write(unsigned char data) {

	epd_spi_read(data);
}

/**
 * \brief Send data to SPI with time out feature
 *
 * \param data The data to be sent out
 */
uint8_t epd_spi_write_ex(unsigned char Data) {
	(void)Data;
	return 0;
}

/**
* \brief SPI command
*
* \param Register The Register Index as SPI Data to COG
* \param Data The Register Data for sending command data to COG
* \return the SPI read value
*/
uint8_t SPI_R(uint8_t Register, uint8_t Data) {

	uint8_t result;
	EPD_cs_low ();
	epd_spi_write (0x70); // header of Register Index
	epd_spi_write (Register);

	EPD_cs_high ();
	Wait_10us ();
	EPD_cs_low ();

	epd_spi_write (0x73); // header of Register Data of read command
	result=epd_spi_read (Data);

	EPD_cs_high ();

	return result;

}

/**
* \brief SPI command if register data is larger than two bytes
*
* \param register_index The Register Index as SPI command to COG
* \param register_data The Register Data for sending command data to COG
* \param length The number of bytes of Register Data which depends on which
* Register Index is selected.
*/
void epd_spi_send (unsigned char register_index, unsigned char *register_data,
               unsigned length) {

	EPD_cs_low ();
	epd_spi_write (0x70); // header of Register Index
	epd_spi_write (register_index);

	EPD_cs_high ();
	Wait_10us ();
	EPD_cs_low ();

	epd_spi_write (0x72); // header of Register Data of write command
	while(length--) {
		epd_spi_write (*register_data++);
	}
	EPD_cs_high ();

}

/**
* \brief SPI command
*
* \param register_index The Register Index as SPI command to COG
* \param register_data The Register Data for sending command data to COG
*/
void epd_spi_send_byte (uint8_t register_index, uint8_t register_data) {

	EPD_cs_low ();
	epd_spi_write (0x70); // header of Register Index
	epd_spi_write (register_index);

	EPD_cs_high ();
	Wait_10us ();
	EPD_cs_low ();
	epd_spi_write (0x72); // header of Register Data
	epd_spi_write (register_data);
	EPD_cs_high ();

}

//******************************************************************
//* Temperature sensor  Configuration
//******************************************************************

/**
 * \brief Get temperature value from ADC
 *
 * \return the Celsius temperature
 */
int16_t get_temperature(void) {

	return 25;
}


//============================================================================================================================================================

//============================================================================================================================================================
/**
* \brief Configure GPIO
*/
void EPD_initialize_gpio(void) {

    /* MAP_PRCMPeripheralClkEnable(PRCM_GPIOA0, PRCM_RUN_MODE_CLK); */
    /* MAP_PRCMPeripheralClkEnable(PRCM_GPIOA1, PRCM_RUN_MODE_CLK); */
    /* MAP_PRCMPeripheralClkEnable(PRCM_GPIOA2, PRCM_RUN_MODE_CLK); */
    /* MAP_PRCMPeripheralClkEnable(PRCM_GPIOA3, PRCM_RUN_MODE_CLK); */

    config_gpio_dir_i(Temper_PinIO,Temper_PORT,Temper_PIN);
    config_gpio_dir_i(EPD_BUSY_PinIO,EPD_BUSY_PORT,EPD_BUSY_PIN);
    config_gpio_dir_o(PWM_PinIO,PWM_PORT,PWM_PIN);
    config_gpio_dir_o(EPD_RST_PinIO,EPD_RST_PORT,EPD_RST_PIN);
    config_gpio_dir_o(EPD_PANELON_PinIO,EPD_PANELON_PORT,EPD_PANELON_PIN);
    config_gpio_dir_o(EPD_DISCHARGE_PinIO,EPD_DISCHARGE_PORT,EPD_DISCHARGE_PIN);
    config_gpio_dir_o(EPD_BORDER_PinIO,EPD_BORDER_PORT,EPD_BORDER_PIN);
    config_gpio_dir_o(Flash_CS_PinIO,Flash_CS_PORT,Flash_CS_PIN);
    config_gpio_dir_o(EPD_CS_PinIO,EPD_CS_PORT,EPD_CS_PIN);
	
	EPD_flash_cs_high();
	EPD_border_low();

}


/**
 * \brief Initialize the EPD hardware setting
 */
void EPD_display_hardware_init(void) {
	
	EPD_initialize_gpio();
	EPD_Vcc_turn_off();
	epd_spi_init();
	EPD_cs_low();
	EPD_pwm_low();
	EPD_rst_low();
	EPD_discharge_low();
	EPD_border_low();
	initialize_EPD_timer();


}
void system_init(void)
{
    /* MAP_PRCMPeripheralClkEnable(PRCM_GPIOA0, PRCM_RUN_MODE_CLK); */
    /* MAP_PRCMPeripheralClkEnable(PRCM_GPIOA1, PRCM_RUN_MODE_CLK); */
    /* MAP_PRCMPeripheralClkEnable(PRCM_GPIOA2, PRCM_RUN_MODE_CLK); */
    /* MAP_PRCMPeripheralClkEnable(PRCM_GPIOA3, PRCM_RUN_MODE_CLK); */
    config_gpio_dir_o(Temper_PinIO,Temper_PORT,Temper_PIN);
    config_gpio_dir_o(SPICLK_PinIO,SPICLK_PORT,SPICLK_PIN);
    config_gpio_dir_o(EPD_BUSY_PinIO,EPD_BUSY_PORT,EPD_BUSY_PIN);
    config_gpio_dir_o(PWM_PinIO,PWM_PORT,PWM_PIN);
    
    config_gpio_dir_o(EPD_PANELON_PinIO,EPD_PANELON_PORT,EPD_PANELON_PIN);
    config_gpio_dir_o(EPD_DISCHARGE_PinIO,EPD_DISCHARGE_PORT,EPD_DISCHARGE_PIN);
    config_gpio_dir_o(EPD_BORDER_PinIO,EPD_BORDER_PORT,EPD_BORDER_PIN);

    config_gpio_dir_o(SPIMISO_PinIO,SPIMISO_PORT,SPIMISO_PIN);
    config_gpio_dir_o(SPIMOSI_PinIO,SPIMOSI_PORT,SPIMOSI_PIN);
    config_gpio_dir_o(Flash_CS_PinIO,Flash_CS_PORT,Flash_CS_PIN);
    config_gpio_dir_o(EPD_CS_PinIO,EPD_CS_PORT,EPD_CS_PIN);
}
