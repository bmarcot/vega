/**
 * \file
 *
 * \brief The initialization and configuration of COG hardware driver
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

#include <math.h>
#include "EPD_hardware_driver.h"

static uint16_t EPD_Counter;
static uint8_t spi_flag = FALSE;

/**
 * \brief Set up EPD Timer for 1 mSec interrupts
 *
 * \note
 * desired value: 1mSec
 * actual value:  1.000mSec
 */
static void initialize_EPD_timer(void) {
	//------------------Timer A1----------------------------------------
	//set up Timer_A CCR1 as master timer using ACLK	
	TACCTL2 &= ~(CCIFG | CCIE); // reset CCIFG Interrupt Flag
	TACCTL = TASSEL_2 + MC_0 + TACLR + ID_3;
	TACCTL2 = OUTMOD_4;
	EPD_Counter = 0;
}

/**
 * \brief Start Timer
 */
void start_EPD_timer(void) {
	initialize_EPD_timer();
	TAR = 0;
	TACCR0 = 990 * 2; // 1ms
	TACCTL2 |= CCIE;
	TACCTL |= MC_1;
	EPD_Counter = 0;
}

/**
 * \brief Stop Timer
 */
void stop_EPD_timer(void) {
	TACCTL2 &= ~CCIE;
	TACCTL &= ~MC_1;
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
void SysTick_Handler(void) {
	EPD_Counter++;
}

/**
 * \brief Interrupt Service Routine for Timer A0
 */
#pragma vector=TIMER0_A1_VECTOR
__interrupt void Timer_A0(void) {
	switch (__even_in_range(TA0IV, 10)) {
	case 2:
		//LPM3_EXIT;
		break;

	case 4:
		EPD_Counter++;
		LPM3_EXIT;
		break;
	}

}

/**
 * \brief Delay mini-seconds
 * \param ms The number of mini-seconds
 */
void delay_ms(unsigned int ms) {
	while (ms--) {
		__delay_cycles(SMCLK_FREQ / 1000);
	}
}

/**
 * \brief Delay mini-seconds
 * \param ms The number of mini-seconds
 */
void sys_delay_ms(unsigned int ms) {
	delay_ms(ms);
}

static void Wait_10us(void) {
	//__delay_cycles(SMCLK_FREQ / 100000);
}

//******************************************************************
//* PWM  Configuration/Control //PWM output : PD3
//******************************************************************

/**
 * \brief The PWM signal starts toggling
 */
void PWM_start_toggle(void) {

}

/**
 * \brief The PWM signal stops toggling.
 */
void PWM_stop_toggle(void) {

}

/**
 * \brief PWM toggling.
 *
 * \param ms The interval of PWM toggling (mini seconds)
 */
void PWM_run(uint16_t ms) {
	start_EPD_timer();
	do {
		EPD_pwm_high();
		__delay_cycles(30);
		EPD_pwm_low();
		__delay_cycles(30);
	} while (get_current_time_tick() < ms); //wait Delay Time
	stop_EPD_timer();
}

//******************************************************************
//* SPI  Configuration
//******************************************************************
/**
 * \brief Configure SPI
 */
void epd_spi_init(void) {
	if (spi_flag)
		return;
	spi_flag = TRUE;
	//config  i/o
	config_gpio_dir_o(SPICLK_PORT, SPICLK_PIN);
	config_gpio_dir_o(SPIMOSI_PORT, SPIMOSI_PIN);
	config_gpio_dir_i(SPIMISO_PORT, SPIMISO_PIN);

	BITSET(SPISEL, SPICLK_PIN + SPIMOSI_PIN + SPIMISO_PIN);
#if(defined __MSP430G2553__)
    BITSET(SPISEL2, SPICLK_PIN + SPIMOSI_PIN + SPIMISO_PIN);
#endif
	//comfig SPI
	SPICTL1 |=UCSWRST;
	SPICTL0 = UCMST+UCSYNC+UCCKPL+UCMSB; 
	SPICTL1 = UCSSEL_2 ;
    SPICTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
	SPIBR0 = SPI_baudrate; //25MHz/2=13.5MHz
	SPIBR1 = 0;

#if(defined __MSP430G2553__)
    BITSET(REN (SPIMISO_PORT), SPIMISO_PIN);
#endif
	BITCLR(SPICTL1, UCSWRST);

}

/**
 * \brief Initialize SPI
 */
void epd_spi_attach(void) {
	EPD_flash_cs_high();
	EPD_cs_high();
	epd_spi_init();
}

/**
 * \brief Disable SPI and change to GPIO
 */
void epd_spi_detach(void) {
	BITCLR(SPISEL, SPICLK_PIN + SPIMOSI_PIN + SPIMISO_PIN);
#if(defined __MSP430G2553__)
    BITCLR(SPISEL2, SPICLK_PIN + SPIMOSI_PIN + SPIMISO_PIN);
#endif
	SPIMISO_low();
	SPIMOSI_low();
	SPICLK_low();
	spi_flag = FALSE;
}

/**
 * \brief SPI synchronous read
 */
uint8_t epd_spi_read(unsigned char RDATA) {
	//while (!(SPIIFG & SPITXIFG));
	SPITXBUF = RDATA;
	while ((SPISTAT & SPIBUSY));
	RDATA=SPIRXBUF;
	return RDATA;

}

/**
 * \brief SPI synchronous write
 */
void epd_spi_write(unsigned char Data) {
	epd_spi_read(Data);
}
/**
 * \brief Send data to SPI with time out feature
 *
 * \param data The data to be sent out
 */
uint8_t epd_spi_write_ex(unsigned char Data) {
	uint8_t cnt = 200;
	uint8_t flag = 1;
	SPITXBUF = Data;
	while (!(SPIIFG & SPITXIFG)) {
		if ((cnt--) == 0) {
			flag = 0;
			break;
		}
	}
	return flag;
}

/**
* \brief SPI command
*
* \param register_index The Register Index as SPI Data to COG
* \param register_data The Register Data for sending command data to COG
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
// ADC12 interrupt service routine

#if (defined __MSP430F5529__)
 #pragma vector=ADC12_VECTOR 
 __interrupt void ADC12_ISR(void) {  
#elif(defined __MSP430G2553__)
#pragma vector=ADC10_VECTOR
__interrupt void ADC10_ISR(void) {    
#endif

	__bic_SR_register_on_exit(CPUOFF);
}

/**
 * \brief Get temperature value from ADC
 *
 * \return the Celsius temperature
 */
int16_t get_temperature(void) {
	float IntDegC;
	const uint8_t DegCOffset = 2;
	long temp;
#if (defined __MSP430F5529__)
    uint8_t i;
    BITSET(ADCCTL0, ADCENC); 
    
    for(i=0;i<4;i++)
    {
    	 ADCCTL0 |= ADCSC;                   // Start convn - software trigger
    	 while (!(ADCIFG & BIT0));
    	 temp = ADCMEM;
    }
    temp=0;
    for(i=0;i<8;i++)
    {
    	 ADCCTL0 |= ADCSC;                   // Start convn - software trigger
    	 while (!(ADCIFG & BIT0));
    	 temp += ADCMEM;
    }	
	temp/=8;

	// IntDegC=(203-DegCOffset)-((long)((7*temp)/128)+(temp/2));
	IntDegC = (201 - DegCOffset) - ((long) ((5 * temp) / 128) + (temp / 2));
    BITCLR(ADCCTL0, ADCENC); 
#elif(defined __MSP430G2553__)
    ADC10CTL1 = INCH_10 + ADC10DIV_3; // Temp Sensor ADC10CLK/4
	ADC10CTL0 = SREF_1 + ADC10SHT_3 + REFON + ADC10ON+ADC10IE;
	ADC10CTL0 |= ENC + ADC10SC;// Sampling and conversion start
	__bis_SR_register(CPUOFF + GIE);// LPM0, ADC10_ISR will force exit
	// oC = ((A10/1024)*1500mV)-986mV)*1/3.55mV = A10*423/1024 - 278
	temp = ADC10MEM;

	ADC10CTL0 |= ENC + ADC10SC;// Sampling and conversion start
	__bis_SR_register(CPUOFF + GIE);// LPM0, ADC10_ISR will force exit
	temp = ADC10MEM;
	ADC10CTL0 |= ENC + ADC10SC;// Sampling and conversion start
	__bis_SR_register(CPUOFF + GIE);// LPM0, ADC10_ISR will force exit
	temp += ADC10MEM;
	temp=temp/2;

	IntDegC =(long)((long)(temp*423)/1024)-(278+DegCOffset);//(long)((long)(temp - 673) * 423) / 1024;

	__no_operation();// SET BREAKPOINT HERE
#endif
	return (int16_t) IntDegC;
	//return 25;
}


/**
 * \brief Initialize the temperature sensor
 */
void initialize_temperature(void) {
#if (defined __MSP430F5529__)
    REFCTL0 &= ~REFMSTR;                      // Reset REFMSTR to hand over control to 
                                             // ADC12_A ref control registers
	ADCCTL0 = ADC12ON+ADC12SHT03+ADC12REFON+ADC12REF2_5V;
                                            // Turn on ADC12, Sampling time
                                            // On Reference Generator and set to
                                            // 2.5V
	ADC12CTL1 = ADC12SHP + ADC12DIV_1+ADC12SSEL_1; 
    ADC12CTL2 =ADC12TCOFF +ADC12RES_1;
	ADC12MCTL0 = ADC12SREF_1+ADC12INCH_6; // input A6
#elif(defined __MSP430G2553__)
	ADC10CTL0 = SREF_1 + ADC10SHT_3 + ADC10ON + ADC10IE + REFON + REF2_5V; // ADC10ON, interrupt enabled
	ADC10CTL1 = INCH_4 + ADC10DIV_3; // input A1.4
#endif
}
//============================================================================================================================================================

#if (defined __MSP430F5529__)
void SetVcoreUp (unsigned int level)
{
  // Open PMM registers for write
  PMMCTL0_H = PMMPW_H;
  // Set SVS/SVM high side new level
  SVSMHCTL = SVSHE + SVSHRVL0 * level + SVMHE + SVSMHRRL0 * level;
  // Set SVM low side to new level
  SVSMLCTL = SVSLE + SVMLE + SVSMLRRL0 * level;
  // Wait till SVM is settled
  while ((PMMIFG & SVSMLDLYIFG) == 0);
  // Clear already set flags
  PMMIFG &= ~(SVMLVLRIFG + SVMLIFG);
  // Set VCore to new level
  PMMCTL0_L = PMMCOREV0 * level;
  // Wait till new level reached
  if ((PMMIFG & SVMLIFG))
    while ((PMMIFG & SVMLVLRIFG) == 0);
  // Set SVS/SVM low side to new level
  SVSMLCTL = SVSLE + SVSLRVL0 * level + SVMLE + SVSMLRRL0 * level;
  // Lock PMM registers for write access
  PMMCTL0_H = 0x00;
}
/**
 * \brief Set CPU work Frequency
 *
 * \param select_index The defined CPU frequency
 * */
void system_init(void) {

  WDTCTL = WDTPW + WDTHOLD; // Stop Watchdog Timer
	// Increase Vcore setting to level3 to support fsystem=25MHz
  // NOTE: Change core voltage one level at a time..
  SetVcoreUp (0x01);
  SetVcoreUp (0x02);
  SetVcoreUp (0x03);

  UCSCTL3 = SELREF_2;                       // Set DCO FLL reference = REFO
  UCSCTL4 |= SELA_2;                        // Set ACLK = REFO

  __bis_SR_register(SCG0);                  // Disable the FLL control loop
  UCSCTL0 = 0x0000;                         // Set lowest possible DCOx, MODx
  UCSCTL1 = DCORSEL_7;                      // Select DCO range 50MHz operation
  UCSCTL2 = FLLD_0 + 762;                   // Set DCO Multiplier for 25MHz
                                            // (N + 1) * FLLRef = Fdco
                                            // (762 + 1) * 32768 = 25MHz
                                            // Set FLL Div = fDCOCLK/2
  __bic_SR_register(SCG0);                  // Enable the FLL control loop

  // Worst-case settling time for the DCO when the DCO range bits have been
  // changed is n x 32 x 32 x f_MCLK / f_FLL_reference. See UCS chapter in 5xx
  // UG for optimization.
  // 32 x 32 x 25 MHz / 32,768 Hz ~ 780k MCLK cycles for DCO to settle
  __delay_cycles(782000);

  // Loop until XT1,XT2 & DCO stabilizes - In this case only DCO has to stabilize
  do
  {
    UCSCTL7 &= ~(XT2OFFG + XT1LFOFFG + DCOFFG);
                                            // Clear XT2,XT1,DCO fault flags
    SFRIFG1 &= ~OFIFG;                      // Clear fault flags
  }while (SFRIFG1&OFIFG);                   // Test oscillator fault flag
}
#elif (defined __MSP430G2553__)
/**
 * \brief Set CPU work Frequency
 *
 * \param select_index The defined CPU frequency
 * */
void system_init(void) {
    WDTCTL = WDTPW + WDTHOLD; // Stop Watchdog Timer
	if (CALBC1_16MHZ == 0xFF || CALDCO_16MHZ == 0xFF) {
			//Info is missing, guess at a good value.
			BCSCTL1 = 0x8f; //CALBC1_16MHZ at 0x10f9
			DCOCTL = 0x9C;  //CALDCO_16MHZ at 0x10f8
		} else {
			BCSCTL1 = CALBC1_16MHZ;
			DCOCTL = CALDCO_16MHZ;
		}
    __delay_cycles(1600000);
}

#endif
//============================================================================================================================================================
/**
* \brief Configure GPIO
*/
void EPD_initialize_gpio(void) {
#if (defined __MSP430F5529__)
    config_gpio_dir_i( EPD_BUSY_PORT,EPD_BUSY_PIN);       
	config_gpio_dir_o( EPD_CS_PORT,EPD_CS_PIN);
	config_gpio_dir_o( EPD_RST_PORT,EPD_RST_PIN);
	config_gpio_dir_o( EPD_PANELON_PORT,EPD_PANELON_PIN);
	config_gpio_dir_o( EPD_DISCHARGE_PORT,EPD_DISCHARGE_PIN);
	config_gpio_dir_o( EPD_BORDER_PORT,EPD_BORDER_PIN);
	config_gpio_dir_o( Flash_CS_PORT,Flash_CS_PIN);
    config_gpio_dir_o( PWM_PORT,PWM_PIN);
	config_gpio_dir_i( Temper_PORT,Temper_PIN);
    BITSET(Temper_PORT_SEL,Temper_PIN);  // Enable A/D channel A0
#elif(defined __MSP430G2553__)
    config_gpio_dir_i( EPD_BUSY_PORT,EPD_BUSY_PIN);
    //Set Xin to GPIO
	BITCLR (CS_PORT_SEL, EPD_CS_PIN);
	BITCLR (CS_PORT_SEL2,EPD_CS_PIN);
    //Set Xout to GPIO
    BITCLR (Flash_CS_PORT_SEL, Flash_CS_PIN);
	BITCLR (Flash_CS_PORT_SEL2,Flash_CS_PIN);
    
	config_gpio_dir_o( EPD_CS_PORT,EPD_CS_PIN);
	config_gpio_dir_o( EPD_RST_PORT,EPD_RST_PIN);
	config_gpio_dir_o( EPD_PANELON_PORT,EPD_PANELON_PIN);
	config_gpio_dir_o( EPD_DISCHARGE_PORT,EPD_DISCHARGE_PIN);
	config_gpio_dir_o( EPD_BORDER_PORT,EPD_BORDER_PIN);
	config_gpio_dir_o( Flash_CS_PORT,Flash_CS_PIN);
    config_gpio_dir_o( PWM_PORT,PWM_PIN);
	config_gpio_dir_i( Temper_PORT,Temper_PIN);
#endif
	
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
	initialize_temperature();
	EPD_cs_low();
	EPD_pwm_low();
	EPD_rst_low();
	EPD_discharge_low();
	EPD_border_low();
	//initialize_EPD_timer();
}

