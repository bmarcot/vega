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
		display_GlobalUpdate(EPD_144,(uint8_t *)&Image_144_Black,(uint8_t *)&Image_144_White,TRUE);

		display_PowerOn(EPD_144);
		display_GlobalUpdate(EPD_144,(uint8_t *)&Image_144_White,(uint8_t *)&Image_144_White,FALSE);

		PartialUpdate_StageTime=100; //There should be obvious ghosting images. Try to extend the stage time to get better optical performance.

		display_PartialUpdate((uint8_t *)&Image_144_White,(uint8_t *)&Image_144_00);
		display_PartialUpdate((uint8_t *)&Image_144_00,(uint8_t *)&Image_144_49);
//		display_PartialUpdate((uint8_t *)&Image_144_59,(uint8_t *)&Image_144_58);
//		display_PartialUpdate((uint8_t *)&Image_144_58,(uint8_t *)&Image_144_57);
//		display_PartialUpdate((uint8_t *)&Image_144_57,(uint8_t *)&Image_144_56);
//		display_PartialUpdate((uint8_t *)&Image_144_56,(uint8_t *)&Image_144_55);
//		display_PartialUpdate((uint8_t *)&Image_144_55,(uint8_t *)&Image_144_54);
//		display_PartialUpdate((uint8_t *)&Image_144_54,(uint8_t *)&Image_144_53);
//		display_PartialUpdate((uint8_t *)&Image_144_53,(uint8_t *)&Image_144_52);
//		display_PartialUpdate((uint8_t *)&Image_144_52,(uint8_t *)&Image_144_51);
//		display_PartialUpdate((uint8_t *)&Image_144_51,(uint8_t *)&Image_144_50);
//		display_PartialUpdate((uint8_t *)&Image_144_50,(uint8_t *)&Image_144_49);
		display_PartialUpdate((uint8_t *)&Image_144_49,(uint8_t *)&Image_144_48);
		display_PartialUpdate((uint8_t *)&Image_144_48,(uint8_t *)&Image_144_47);
		display_PartialUpdate((uint8_t *)&Image_144_47,(uint8_t *)&Image_144_46);
		display_PartialUpdate((uint8_t *)&Image_144_46,(uint8_t *)&Image_144_45);
		display_PartialUpdate((uint8_t *)&Image_144_45,(uint8_t *)&Image_144_44);
		display_PartialUpdate((uint8_t *)&Image_144_44,(uint8_t *)&Image_144_43);
		display_PartialUpdate((uint8_t *)&Image_144_43,(uint8_t *)&Image_144_42);
		display_PartialUpdate((uint8_t *)&Image_144_42,(uint8_t *)&Image_144_41);
		display_PartialUpdate((uint8_t *)&Image_144_41,(uint8_t *)&Image_144_40);
		display_PartialUpdate((uint8_t *)&Image_144_40,(uint8_t *)&Image_144_39);
		display_PartialUpdate((uint8_t *)&Image_144_39,(uint8_t *)&Image_144_38);
		display_PartialUpdate((uint8_t *)&Image_144_38,(uint8_t *)&Image_144_37);
		display_PartialUpdate((uint8_t *)&Image_144_37,(uint8_t *)&Image_144_36);
		display_PartialUpdate((uint8_t *)&Image_144_36,(uint8_t *)&Image_144_35);
		display_PartialUpdate((uint8_t *)&Image_144_35,(uint8_t *)&Image_144_34);
		display_PartialUpdate((uint8_t *)&Image_144_34,(uint8_t *)&Image_144_33);
		display_PartialUpdate((uint8_t *)&Image_144_33,(uint8_t *)&Image_144_32);
		display_PartialUpdate((uint8_t *)&Image_144_32,(uint8_t *)&Image_144_31);
		display_PartialUpdate((uint8_t *)&Image_144_31,(uint8_t *)&Image_144_30);
		display_PartialUpdate((uint8_t *)&Image_144_30,(uint8_t *)&Image_144_29);
		display_PartialUpdate((uint8_t *)&Image_144_29,(uint8_t *)&Image_144_28);
		display_PartialUpdate((uint8_t *)&Image_144_28,(uint8_t *)&Image_144_27);
		display_PartialUpdate((uint8_t *)&Image_144_27,(uint8_t *)&Image_144_26);
		display_PartialUpdate((uint8_t *)&Image_144_26,(uint8_t *)&Image_144_25);
		display_PartialUpdate((uint8_t *)&Image_144_25,(uint8_t *)&Image_144_24);
		display_PartialUpdate((uint8_t *)&Image_144_24,(uint8_t *)&Image_144_23);
		display_PartialUpdate((uint8_t *)&Image_144_23,(uint8_t *)&Image_144_22);
		display_PartialUpdate((uint8_t *)&Image_144_22,(uint8_t *)&Image_144_21);
		display_PartialUpdate((uint8_t *)&Image_144_21,(uint8_t *)&Image_144_20);
		display_PartialUpdate((uint8_t *)&Image_144_20,(uint8_t *)&Image_144_19);
		display_PartialUpdate((uint8_t *)&Image_144_19,(uint8_t *)&Image_144_18);
		display_PartialUpdate((uint8_t *)&Image_144_18,(uint8_t *)&Image_144_17);
		display_PartialUpdate((uint8_t *)&Image_144_17,(uint8_t *)&Image_144_16);
		display_PartialUpdate((uint8_t *)&Image_144_16,(uint8_t *)&Image_144_15);
		display_PartialUpdate((uint8_t *)&Image_144_15,(uint8_t *)&Image_144_14);
		display_PartialUpdate((uint8_t *)&Image_144_14,(uint8_t *)&Image_144_13);
		display_PartialUpdate((uint8_t *)&Image_144_13,(uint8_t *)&Image_144_12);
		display_PartialUpdate((uint8_t *)&Image_144_12,(uint8_t *)&Image_144_11);
		display_PartialUpdate((uint8_t *)&Image_144_11,(uint8_t *)&Image_144_10);
		display_PartialUpdate((uint8_t *)&Image_144_10,(uint8_t *)&Image_144_09);
		display_PartialUpdate((uint8_t *)&Image_144_09,(uint8_t *)&Image_144_08);
		display_PartialUpdate((uint8_t *)&Image_144_08,(uint8_t *)&Image_144_07);
		display_PartialUpdate((uint8_t *)&Image_144_07,(uint8_t *)&Image_144_06);
		display_PartialUpdate((uint8_t *)&Image_144_06,(uint8_t *)&Image_144_05);
		display_PartialUpdate((uint8_t *)&Image_144_05,(uint8_t *)&Image_144_04);
		display_PartialUpdate((uint8_t *)&Image_144_04,(uint8_t *)&Image_144_03);
		display_PartialUpdate((uint8_t *)&Image_144_03,(uint8_t *)&Image_144_02);
		display_PartialUpdate((uint8_t *)&Image_144_02,(uint8_t *)&Image_144_01);
		display_PartialUpdate((uint8_t *)&Image_144_01,(uint8_t *)&Image_144_00);
		display_PowerOff();
		//delay_ms(1000);
	}

}

