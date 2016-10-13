/**
 * \file
 *
 * \brief Sample project code for demonstrating Pervasive Displays 1.44", 1.9", 2", 2.6" and 2.7" EPD
 *
 * \note There is quick start guide at the bottom of this file
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

/**
 * \brief Demonstration on EPD Extension board with EPD for toggling between two images
 *
 */

#include "conf_EPD.h"
#include "image_data.h"

//*****************************************************************************
//                 GLOBAL VARIABLES -- Start
//*****************************************************************************
#if defined(ccs)
extern void (* const g_pfnVectors[])(void);
#endif
#if defined(ewarm)
extern uVectorEntry __vector_table;
#endif
//*****************************************************************************
//                 GLOBAL VARIABLES -- End
//*****************************************************************************


//*****************************************************************************
//                      LOCAL FUNCTION PROTOTYPES                           
//*****************************************************************************
static void BoardInit(void);

//*****************************************************************************
//                      LOCAL FUNCTION DEFINITIONS                         
//*****************************************************************************


//*****************************************************************************
//
//! Board Initialization & Configuration
//!
//! \param  None
//!
//! \return None
//
//*****************************************************************************
static void
BoardInit(void)
{
/* In case of TI-RTOS vector table is initialize by OS itself */
#ifndef USE_TIRTOS
    //
    // Set vector table base
    //
#if defined(ccs)
    MAP_IntVTableBaseSet((unsigned long)&g_pfnVectors[0]);
#endif
#if defined(ewarm)
    MAP_IntVTableBaseSet((unsigned long)&__vector_table);
#endif
#endif
    
    //
    // Enable Processor
    //
    MAP_IntMasterEnable();
    MAP_IntEnable(FAULT_SYSTICK);

    PRCMCC3200MCUInit();
}

int
main()
{
    //
    // Initialize Board configurations
    //
    BoardInit();

  //  PinMuxConfig();
    EPD_display_hardware_init();
    for(;;) {
    	/* User selects which EPD size to run demonstration by changing the
    		 * USE_EPD_Type in conf_EPD.h
    		 * The Image data arrays for each EPD size are predefined at image_data.c */
    #if(USE_EPD_Type==EPD_144)
    		EPD_display_from_pointer(EPD_144,(uint8_t *)&image_array_144_2,(uint8_t *)&image_array_144_1);
    #elif(USE_EPD_Type==EPD_200)
    		EPD_display_from_pointer(EPD_200,(uint8_t *)&image_array_200_2,(uint8_t *)&image_array_200_1);
    #elif(USE_EPD_Type==EPD_270)
    		/* Due to if loading two image arrays for 2.7", the code size will exceed the MSP430G2553 flash.
    		 * So the demo will load 1st image continuously.*/
    #ifdef G2_Aurora_Ma
    		EPD_display_from_pointer(EPD_270,(uint8_t *)&image_array_270_1,(uint8_t *)&image_array_270_1);
    #else
    		EPD_display_from_pointer(EPD_270,(uint8_t *)&image_array_270_2,(uint8_t *)&image_array_270_1);
    #endif
    #elif(USE_EPD_Type==EPD_190)
            EPD_display_from_pointer(EPD_190,(uint8_t *)&image_array_190_2,(uint8_t *)&image_array_190_1);
    #elif(USE_EPD_Type==EPD_260)
            EPD_display_from_pointer(EPD_260,(uint8_t *)&image_array_260_2,(uint8_t *)&image_array_260_1);
    #endif

    		/* The interval of two images alternatively change is 5 seconds */
    		delay_ms(5000);

    #if(USE_EPD_Type==EPD_144)
    		EPD_display_from_pointer(EPD_144,(uint8_t *)&image_array_144_1,(uint8_t *)&image_array_144_2);
    #elif(USE_EPD_Type==EPD_200)
    		EPD_display_from_pointer(EPD_200,(uint8_t *)&image_array_200_1,(uint8_t *)&image_array_200_2);
    #elif(USE_EPD_Type==EPD_270)
    		/* Due to if loading two image arrays for 2.7", the code size will exceed the MSP430G2553 flash.
    		 * So the demo will load 1st image continuously.*/
    #ifdef G2_Aurora_Ma
    		EPD_display_from_pointer(EPD_270,(uint8_t *)&image_array_270_1,(uint8_t *)&image_array_270_1);
    #else
    		EPD_display_from_pointer(EPD_270,(uint8_t *)&image_array_270_1,(uint8_t *)&image_array_270_2);
    #endif
    #elif(USE_EPD_Type==EPD_190)
            EPD_display_from_pointer(EPD_190,(uint8_t *)&image_array_190_1,(uint8_t *)&image_array_190_2);
    #elif(USE_EPD_Type==EPD_260)
            EPD_display_from_pointer(EPD_260,(uint8_t *)&image_array_260_1,(uint8_t *)&image_array_260_2);
    #endif

    		/* The interval of two images alternatively change is 5 seconds */
    		delay_ms(5000);
    	}

}


/**
 * \page - Quick Start Guide
 *
 * This is the quick start guide for the EPD Extension board made by Pervasive Displays Inc.
 * with its small size EPDs on how to setup the kit to CC3200 LaunchPads(CC3200 LAUNCHXL rev.3.2).
 * The code example in main.c provides the sample code toggling between two images from image array.
 * It also instructs how to use the display functions.
 *
 * \note
 * - Released Date: 18 May, 2015.  Version: 2.02
 * - Compiled by CCStudio version = v6.0.1 with ARM compiler v5.2.0
 * - PDi = Pervasive Displays Inc. http://www.pervasivedisplays.com
 * - EPD = Electronic Paper Display (Electrophoretic Display)
 * - EPD Extension Board/Kit = The driving board has direct socket to LaunchPad and
 *   also features 20 pin-out bridges to your product or development kit
 * - COG = Chip on Glass, the driver IC on EPD module
 * - COG G1 or G2: G is for generation.
 * - FPL = Front Plane Laminate which is E-Ink material film.
 *   There are Vizplex(V110, EOL already), Aurora Ma(V230) and Aurora Mb(V231) type
 * - PDi offers Aurora_Ma and Aurora_Mb material with G2 COG to the market.
 *   Some customers got our Aurora_Mb+G1 combination. The sample code is also provided.
 * - Basically, the Aurora_Mb+G1 is the replacement for Vizplex+G1.
 * - How to identify the FPL material type of your displays, please visit
 *   http://www.pervasivedisplays.com/products/label_info
 * - For driving PDi's small size EPDs, please read the "COG Driver Interface
 *   Timing" document(hereinafter COG Document) first. It explains the interface
 *   to the COG driver of EPD for a MCU based solution.
 * - COG Document no.: 4P008-00 (for Vizplex+G1)  : http://www.pervasivedisplays.com/LiteratureRetrieve.aspx?ID=138408
 * - COG Document no.: 4P015-00 (for Aurora_Ma+G2): http://www.pervasivedisplays.com/LiteratureRetrieve.aspx?ID=198794
 * - COG Document no.: 4P016-00 (for Aurora_Mb+G1): http://www.pervasivedisplays.com/LiteratureRetrieve.aspx?ID=220874
 * - COG Document no.: 4P018-00 (for Aurora_Mb+G2): http://www.pervasivedisplays.com/LiteratureRetrieve.aspx?ID=220873
 * - This project code supports EPD size: 1.44", 1.9", 2", 2.6" and 2.7"
 * - Supports CC3200 LaunchPad: CC3200-LAUNCHXL http://www.ti.com/ww/en/launchpad/launchpads-connected-cc3200-launchxl.html#tabs
 *
 * \section File_Explanation
 * - <b>image_data:</b>\n
 *   It defines the image arrays for each EPD size. User can use the array without
 *   application input.
 * - <b>conf_EPD.h:</b> The EPD configurations.\n
 *   -# USE_EPD_Type: if commented out the define of EPD_KIT_TOOL_FUNCTIONS, must define this size of EPD
 *   -# Gx_Aurora_xx: define which FPL material with COG driving waveform of the EPD you're connecting
 *   -# COG_SPI_baudrate: SPI speed, G1 works in 4-12MHz, G2 works in 4-20MHz
 * - <b>Pervasive_Displays_small_EPD</b> folder:\n
 *   All of the COG driving waveforms are located in this folder. Logically developer
 *   doesn't need to change the codes in this folder in order to keep correct driving
 *   to the EPDs.\n\n
 *   <b><em>Software architecture:</em></b>\n
 *   [Application (ex. EPD Kit Tool)] <-- [COG interface (<em>EPD_interface</em>)] <--
 *   [COG driving process (<em>EPD_Gx_Aurora_Mx</em> in COG_FPL folder)] <--
 *   [Hardware Driver & GPIO (<em>EPD_hardware_driver</em>)]\n\n
 *    -# <b>EPD_hardware_driver:</b>\n
 *       Most of the COG hardware initialization, GPIO and configuration. User can implement
 *       the driver layer of EPD if some variables need to be adjusted. The provided
 *       settings and functions are Timer, SPI, PWM, temperature and EPD hardware initialization.
 *    -# <b>COG_FPL</b> folder:\n
 *       The driving process for each sub-folder represents the different display module.
 *       - <b>EPD_UpdateMethod_Gx_Aurora_Mx:</b>\n
 *         UpdateMethod: Global update or Partial update. If none, it means both.
 *         Gx: G1 or G2.
 *         Aurora_Mx: Aurora_Ma or Aurora_Mb.
 *    -# <b>EPD_interface:</b>\n
 *       The application interfaces to work with EPD.
 *
 *
 * \section Steps
 * -# Ensure the EPD is connected correctly on the EPD Extension board
 * -# Stack the EPD Extension board onto CC3200 LaunchPad board directly
 * -# Connect the LaunchPad board to computer's USB port via its USB cable
 * -# Find #define Gx_Aurora_Mx in "conf_EPD.h" file. Change to the correct type of EPD you are connecting.
 * -# Open "conf_EPD.h" file and find "#define USE_EPD_Type USE_EPDxxx". Change the USE_EPDXXX to the correct size.
 * -# Start debugging to program the project code to CC3200 MCU. The EPD will show two images change alternately every 5 seconds (default).
 *
 *
 * \section PDi EPD displays
 * ======================================
 * | Size | PDi Model  |   FPL + COG    |
 * |------|------------|----------------|
 * | 1.44 | EK014BS011 | Aurora_Ma + G2 |
 * | 2.0  | EG020BS011 | Aurora_Ma + G2 |
 * | 2.7  | EM027BS013 | Aurora_Ma + G2 |
 * | 1.44 | EK014CS011 | Aurora_Mb + G1 |
 * | 1.9  | EB019CS011 | Aurora Mb + G1 |
 * | 2.0  | EG020CS012 | Aurora_Mb + G1 |
 * | 2.6  | EN026CS011 | Aurora Mb + G1 |
 * | 2.7  | EM027CS011 | Aurora_Mb + G1 |
 * | 1.44 | E1144CS021 | Aurora Mb + G2 |
 * | 1.9  | E1190CS021 | Aurora Mb + G2 |
 * | 2.0  | E1200CS021 | Aurora Mb + G2 |
 * | 2.6  | E1260CS021 | Aurora Mb + G2 |
 * | 2.7  | E1271CS021 | Aurora Mb + G2 |
 *
 * http://www.pervasivedisplays.com/products/label_info
 */
