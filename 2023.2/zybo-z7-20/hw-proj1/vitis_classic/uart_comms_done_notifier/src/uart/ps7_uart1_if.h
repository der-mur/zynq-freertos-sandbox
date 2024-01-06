/******************************************************************************
 * @Title		:	PS7 UART 1 Interface  (Header File)
 * @Filename	:	ps7_uart1_if.h
 * @Author		:	Derek Murray
 * @Origin Date	:	27/12/2023
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

#ifndef SRC_UART_PS7_UART1_IF_H_
#define SRC_UART_PS7_UART1_IF_H_


/***************************** Include Files ********************************/

/* Xilinx low-level */
#include "xuartps.h"
#include "xscugic.h"



/*****************************************************************************/
/************************** Constant Definitions *****************************/
/*****************************************************************************/

#define UART1_DEBUG						0

/* Device ID */
#define PS7_UART1_DEVICE_ID			XPAR_PS7_UART_1_DEVICE_ID

/* Interrupt Parameters */
#define UART1_INTR_ID				XPS_UART1_INT_ID 	// PS7 UART1, 82U
#define UART1_INTR_PRI				(0xB0)
#define UART1_INTR_TRIG				(0x01) // Active-high Level Sensitive





/* Ensure xil_printf.h is included: */
#ifndef SRC_XIL_PRINTF_H
#define SRC_XIL_PRINTF_H
#endif

#define printf 		xil_printf 	/* Small foot-print printf function */


/****************************************************************************/
/************************** Function Prototypes *****************************/
/****************************************************************************/

/* Device Initialization */
/* xUartPs1Init must be passed by reference from calling code. */

int xUartPs1Init(XUartPs* p_XUartPsInst, XScuGic* xInterruptController);




#endif /* SRC_UART_PS7_UART1_IF_H_ */
