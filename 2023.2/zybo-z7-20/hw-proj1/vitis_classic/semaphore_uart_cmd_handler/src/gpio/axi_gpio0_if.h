/******************************************************************************
 * @Title		:	AXI GPIO Interface (Header File)
 * @Filename	:	axi_gpio0_if.h
 * @Author		:	Derek Murray
 * @Origin Date	:	20/12/2023
 * @Version		:	1.0.0
 * @Compiler	:	arm-none-eabi-gcc
 * @Target		: 	Xilinx Zynq-7000
 * @Platform	: 	Digilent Zybo-Z7-20
 *
 * ------------------------------------------------------------------------
 *
 * Copyright (C) 2024  Derek Murray
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

#ifndef SRC_GPIO_AXI_GPIO0_IF_H_
#define SRC_GPIO_AXI_GPIO0_IF_H_


/*****************************************************************************/
/***************************** Include Files *********************************/
/*****************************************************************************/

#include "xgpio.h"


/*****************************************************************************/
/************************** Constant Definitions *****************************/
/*****************************************************************************/

#define AXI_GPIO0_DEVICE_ID			XPAR_AXI_GPIO_0_DEVICE_ID


/*****************************************************************************/
/******************************* Typedefs ************************************/
/*****************************************************************************/

/* ----- AXI GPIO 0 -----
 *//**
 * AXI GPIO Channel 1 = Output
 * AXI GPIO Channel 2 = Input channel.
 * MASK = 0x0F means lower 4 bits of channel 2 are set to input. */
#define AXI_GPIO0_OP_CHANNEL 		1U
#define AXI_GPIO0_IP_CHANNEL 		2U
#define AXI_GPIO0_IP_MASK			(0x0000000F)



typedef enum {
	LED0, LED1, LED2, LED3,
	PMOD_JE_1, PMOD_JE_2, PMOD_JE_3, PMOD_JE_4,
}AxiGpio0_OutPin_t;


typedef enum { BTN0, BTN1,	SW0, SW1, } AxiGpio0_InPin_t;




/*****************************************************************************/
/************************** Function Prototypes ******************************/
/*****************************************************************************/

/* Device Initialisation */
int axiGpio0Init(void);


/* Interface functions */
void axiGp0_OutSet(AxiGpio0_OutPin_t pin);
void axiGp0_OutClear(AxiGpio0_OutPin_t pin);
void axiGp0_OutToggle(AxiGpio0_OutPin_t pin);
uint32_t axiGp0_InRead(AxiGpio0_InPin_t pin);


#endif /* SRC_GPIO_AXI_GPIO0_IF_H_ */
