/******************************************************************************
 * @Title		:	PS7 SCUGIC Interface (Header File)
 * @Filename	:	ps7_scugic_if.h
 * @Author		:	Derek Murray
 * @Origin Date	:	27/12/2023
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

#ifndef SRC_PS7_SCUGIC_IF_H_
#define SRC_PS7_SCUGIC_IF_H_



/*****************************************************************************/
/***************************** Include Files *********************************/
/*****************************************************************************/

/* Xilinx low-level */
#include "xscugic.h"
#include "xil_exception.h"



/*****************************************************************************/
/************************** Constant Definitions *****************************/
/*****************************************************************************/

#define INTR_SYS_DEBUG				0

/* Device ID */
#define PS7_SCUGIC_DEVICE_ID		XPAR_PS7_SCUGIC_0_DEVICE_ID



/*****************************************************************************/
/************************** Variable Declarations ****************************/
/*****************************************************************************/




/*****************************************************************************/
/************************ Macros (Inline Functions) **************************/
/*****************************************************************************/

/* Ensure xil_printf.h is included: */
#ifndef SRC_XIL_PRINTF_H
#define SRC_XIL_PRINTF_H
#endif

#define printf 		xil_printf 	/* Small foot-print printf function */



/*****************************************************************************/
/************************** Function Prototypes ******************************/
/*****************************************************************************/

/* Device Initialization */
/* p_XScuGicInst must be passed by reference from calling code. */
int xScuGicInit(XScuGic* p_XScuGicInst);


/* Interface functions */
void enableInterrupts(void);
void disableInterrupts(void);


#endif /* SRC_PS7_SCUGIC_IF_H_ */
