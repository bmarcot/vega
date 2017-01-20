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
		PartialUpdate_StageTime=630;
		display_GlobalUpdate(EPD_260,(uint8_t *)&Image_26_Black,(uint8_t *)&Image_26_White,TRUE);

		display_PowerOn(EPD_260);
		display_GlobalUpdate(EPD_260,(uint8_t *)&Image_26_White,(uint8_t *)&Image_26_White,FALSE);

		display_PartialUpdate((uint8_t *)&Image_26_White,(uint8_t *)&Image_26_ECG01);
		display_PartialUpdate((uint8_t *)&Image_26_ECG01,(uint8_t *)&Image_26_ECG02);
		display_PartialUpdate((uint8_t *)&Image_26_ECG02,(uint8_t *)&Image_26_ECG03);
		display_PartialUpdate((uint8_t *)&Image_26_ECG03,(uint8_t *)&Image_26_ECG04);
		display_PartialUpdate((uint8_t *)&Image_26_ECG04,(uint8_t *)&Image_26_ECG05);
		display_PartialUpdate((uint8_t *)&Image_26_ECG05,(uint8_t *)&Image_26_ECG06);
		display_PartialUpdate((uint8_t *)&Image_26_ECG06,(uint8_t *)&Image_26_ECG07);
		display_PartialUpdate((uint8_t *)&Image_26_ECG07,(uint8_t *)&Image_26_ECG08);
		display_PartialUpdate((uint8_t *)&Image_26_ECG08,(uint8_t *)&Image_26_ECG09);
		display_PartialUpdate((uint8_t *)&Image_26_ECG09,(uint8_t *)&Image_26_ECG10);
		display_PartialUpdate((uint8_t *)&Image_26_ECG10,(uint8_t *)&Image_26_ECG11);
		display_PartialUpdate((uint8_t *)&Image_26_ECG11,(uint8_t *)&Image_26_ECG12);
		display_PartialUpdate((uint8_t *)&Image_26_ECG12,(uint8_t *)&Image_26_ECG13);
		display_PartialUpdate((uint8_t *)&Image_26_ECG13,(uint8_t *)&Image_26_ECG14);
		display_PartialUpdate((uint8_t *)&Image_26_ECG14,(uint8_t *)&Image_26_ECG15);
		display_PartialUpdate((uint8_t *)&Image_26_ECG15,(uint8_t *)&Image_26_ECG16);

		display_PowerOff();
		delay_ms(3000);
	}

}
