int v231_power_off(void)
{
	if ((cur_EPD_type_index == EPD_190) || (cur_EPD_type_index == EPD_260))
		nothing_frame_for_19_26();
	else
		nothing_frame();

	if (cur_EPD_type_index == EPD_270) {
		dummy_line();
		delay_ms (25);
		EPD_border_low();
		delay_ms (200);
		EPD_border_high();
	} else {
		border_dummy_line();
		delay_ms (200);
	}

	//Check DC/DC
	if ((SPI_R(0x0F, 0x00) & 0x40) == 0)
		return ERROR_DC;

	SPI(0x0B, 0, 1);

	//Turn on Latch Reset
	SPI(0x03, 0x01, 1);

	//Power off charge pump Vcom
	SPI(0x05, 0x03, 1);

	//Power off charge pump neg voltage
	SPI(0x05, 0x01, 1);
	delay_ms(120);

	//Discharge internal SPI
	SPI(0x04, 0x80, 1);

	//Turn off all charge pump
	SPI(0x05, 0, 1);

	//Turn off OSC
	SPI(0x07, 0x01, 1);
	delay_ms(50);

	//FIXME: Check it resets Pins qccordingly to POWER_ON()
	epd_spi_detach ();
	EPD_Vcc_turn_off ();
	EPD_border_low();
	delay_ms (10);
	EPD_cs_low();
	EPD_rst_low();

	EPD_discharge_high ();
	delay_ms (150);
	EPD_discharge_low ();

	return 0;
}
