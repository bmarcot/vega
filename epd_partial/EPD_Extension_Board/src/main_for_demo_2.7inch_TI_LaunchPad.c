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
	int8_t i;
	/* Initialize system clock and TI LaunchPad board */
	system_init();
	/* Initialize EPD hardware */
	EPD_display_hardware_init();
	__enable_interrupt();//enable interrupts

	for(;;)
	{
		PartialUpdate_StageTime=630;
		display_GlobalUpdate(EPD_270,(uint8_t *)&Image_27_Black,(uint8_t *)&Image_27_White,TRUE);

		display_PowerOn(EPD_270);
		display_GlobalUpdate(EPD_270,(uint8_t *)&Image_27_White,(uint8_t *)&Image_27_White,FALSE);

		PartialUpdate_StageTime=350;

		for (i=0;i<3;i++)
		{
			display_PartialUpdate((uint8_t *)&Image_27_White,(uint8_t *)&Image_27_LP01);
			display_PartialUpdate((uint8_t *)&Image_27_LP01,(uint8_t *)&Image_27_LP02);
			display_PartialUpdate((uint8_t *)&Image_27_LP02,(uint8_t *)&Image_27_LP03);
			display_PartialUpdate((uint8_t *)&Image_27_LP03,(uint8_t *)&Image_27_LP04);
			display_PartialUpdate((uint8_t *)&Image_27_LP04,(uint8_t *)&Image_27_LP05);
			display_PartialUpdate((uint8_t *)&Image_27_LP05,(uint8_t *)&Image_27_LP06);
			display_PartialUpdate((uint8_t *)&Image_27_LP06,(uint8_t *)&Image_27_LP07);
			display_PartialUpdate((uint8_t *)&Image_27_LP07,(uint8_t *)&Image_27_LP08);
			display_PartialUpdate((uint8_t *)&Image_27_LP08,(uint8_t *)&Image_27_LP09);
			delay_ms(3000);
			if (i<2) display_GlobalUpdate(EPD_270,(uint8_t *)&Image_27_White,(uint8_t *)&Image_27_White,FALSE);
		}

		display_PowerOff();
		delay_ms(1000);
	}

}
