/**
* \file
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

#ifndef 	EPD_INTERFACE_H_INCLUDED
#define 	EPD_INTERFACE_H_INCLUDED
#include	"conf_EPD.h"

void EPD_display_init(void);
void EPD_power_init(uint8_t EPD_type_index);
void EPD_display_from_pointer(uint8_t EPD_type_index,uint8_t *previous_image_ptr,
	uint8_t *new_image_ptr);
void EPD_display_from_flash(uint8_t EPD_type_index,long previous_image_address,
	long new_image_address,EPD_read_flash_handler On_EPD_read_flash);
void EPD_display_from_flash_Ex(uint8_t EPD_type_index,long previous_image_address,
	long new_image_address,EPD_read_flash_handler On_EPD_read_flash);

#endif 	//DISPLAY_CONTROLLER_H_INCLUDED
