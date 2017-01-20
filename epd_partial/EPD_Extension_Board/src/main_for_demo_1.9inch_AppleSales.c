/**
 * \file
 *
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


#include "conf_EPD.h"
#include "image_data.h"

int main(void) {
	/* Initialize system clock and TI LaunchPad board */
	system_init();
	/* Initialize EPD hardware */
	EPD_display_hardware_init();
	__enable_interrupt();//enable interrupts

	for(;;)
	{
		PartialUpdate_StageTime=480;
		display_GlobalUpdate(EPD_190,(uint8_t *)&Image_19_black,(uint8_t *)&Image_19_white,TRUE);

		display_PowerOn(EPD_190);
		display_GlobalUpdate(EPD_190,(uint8_t *)&Image_19_white,(uint8_t *)&Image_19_white,FALSE);

		PartialUpdate_StageTime=250;

		display_PartialUpdate((uint8_t *)&Image_19_white,(uint8_t *)&Image_19_Apple01);
		display_PartialUpdate((uint8_t *)&Image_19_Apple01,(uint8_t *)&Image_19_Apple02);
		display_PartialUpdate((uint8_t *)&Image_19_Apple02,(uint8_t *)&Image_19_Apple01);
		display_PartialUpdate((uint8_t *)&Image_19_Apple01,(uint8_t *)&Image_19_Apple02);
		display_PartialUpdate((uint8_t *)&Image_19_Apple02,(uint8_t *)&Image_19_Apple01);
		PartialUpdate_StageTime=480;
		display_PartialUpdate((uint8_t *)&Image_19_Apple01,(uint8_t *)&Image_19_Apple03);
		display_PartialUpdate((uint8_t *)&Image_19_Apple03,(uint8_t *)&Image_19_Apple01);
		display_PartialUpdate((uint8_t *)&Image_19_Apple01,(uint8_t *)&Image_19_Apple03);
		display_PartialUpdate((uint8_t *)&Image_19_Apple03,(uint8_t *)&Image_19_Apple04);
		delay_ms(1000);
		display_GlobalUpdate(EPD_190,(uint8_t *)&Image_19_Apple04,(uint8_t *)&Image_19_Apple02,FALSE);
		display_PartialUpdate((uint8_t *)&Image_19_Apple02,(uint8_t *)&Image_19_Apple01);
		display_PartialUpdate((uint8_t *)&Image_19_Apple01,(uint8_t *)&Image_19_Apple05);
		display_PartialUpdate((uint8_t *)&Image_19_Apple05,(uint8_t *)&Image_19_Apple06);
		display_PartialUpdate((uint8_t *)&Image_19_Apple06,(uint8_t *)&Image_19_Apple07);
		display_PartialUpdate((uint8_t *)&Image_19_Apple07,(uint8_t *)&Image_19_Apple08);
		display_PartialUpdate((uint8_t *)&Image_19_Apple08,(uint8_t *)&Image_19_Apple09);
		display_PartialUpdate((uint8_t *)&Image_19_Apple09,(uint8_t *)&Image_19_Apple08);
		display_PartialUpdate((uint8_t *)&Image_19_Apple08,(uint8_t *)&Image_19_Apple01);
		display_PartialUpdate((uint8_t *)&Image_19_Apple01,(uint8_t *)&Image_19_Apple08);
		display_PartialUpdate((uint8_t *)&Image_19_Apple08,(uint8_t *)&Image_19_Apple01);
		display_PartialUpdate((uint8_t *)&Image_19_Apple01,(uint8_t *)&Image_19_Apple08);
		delay_ms(3000);

		display_PowerOff();
		delay_ms(1000);
	}

}
