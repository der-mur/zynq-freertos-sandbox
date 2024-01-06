/******************************************************************************
 * @Title		:	AXI GPIO Interface (Header File)
 * @Filename	:	axi_gpio0_if.h
 * @Author		:	Derek Murray
 * @Origin Date	:	15/05/2020
 * @Version		:	1.0.0
 * @Compiler	:	arm-none-eabi-gcc
 * @Target		: 	Xilinx Zynq-7000
 * @Platform	: 	Digilent Zybo-Z7-20
 *
 * ------------------------------------------------------------------------
 *
 * Copyright (C) 2021  Derek Murray
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
******************************************************************************/

#ifndef SRC_GPIO_AXI_GPIO1_IF_H_
#define SRC_GPIO_AXI_GPIO1_IF_H_


/*****************************************************************************/
/***************************** Include Files *********************************/
/*****************************************************************************/

#include "xgpio.h"


/*****************************************************************************/
/************************** Constant Definitions *****************************/
/*****************************************************************************/

#define AXI_GPIO1_DEVICE_ID			XPAR_AXI_GPIO_1_DEVICE_ID


/*****************************************************************************/
/******************************* Typedefs ************************************/
/*****************************************************************************/

/* ----- AXI GPIO 1 -----
 *//**
 * AXI GPIO Channel 1 = Output
 * AXI GPIO Channel 2 = Input channel.
 * MASK = 0x0F means lower 4 bits of channel 2 are set to input. */
#define AXI_GPIO1_OP_CHANNEL 		1U
#define AXI_GPIO1_IP_CHANNEL 		2U
#define AXI_GPIO1_IP_MASK			(0x0000000F)


/* ----------------------------------------------------------------------------
 * ----- AXI GPIO 0 -----
 *//**
 * For this project, channel 1 is the output channel.
 * 8 outputs are used: 4 x Board LEDS 0-3; 4 x GP_OUT 0-3 (PMOD JE Pins 1-4)
 * Channel 2 is the input channel. 12 inputs are used:
 * 4 x Board BTNS 0-3; 4 x Board SW 0-3; 4 x GP_IN 0-3 (PMOD JE pins 7-10)
 * --------------------------------------------------------------------------*/

typedef enum {
	LD0, LD1, LD2, LD3,	LD4, LD5, LD6, LD7,
	PMOD_JE_7, PMOD_JE_8, PMOD_JE_9, PMOD_JE_10,
}AxiGpio1_OutPin_t;


typedef enum { BTN2, BTN3, SW2, SW3} AxiGpio1_InPin_t;




/*****************************************************************************/
/************************** Function Prototypes ******************************/
/*****************************************************************************/

/* Device Initialisation */
int axiGpio1Init(void);


/* Interface functions */
void axiGp1_OutSet(AxiGpio1_OutPin_t pin);
void axiGp1_OutClear(AxiGpio1_OutPin_t pin);
void axiGp1_OutToggle(AxiGpio1_OutPin_t pin);
uint32_t axiGp1_InRead(AxiGpio1_InPin_t pin);


#endif /* SRC_GPIO_AXI_GPIO1_IF_H_ */
