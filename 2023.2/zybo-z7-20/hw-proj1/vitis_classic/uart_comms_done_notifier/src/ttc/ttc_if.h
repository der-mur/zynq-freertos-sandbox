/******************************************************************************
 * @Title		:	Triple Timer Counter Interface (Header File)
 * @Filename	:	ttc_if.h
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

#ifndef SRC_TIMERS_TTC0_IF_H_
#define SRC_TIMERS_TTC0_IF_H_



/*****************************************************************************/
/***************************** Include Files *********************************/
/*****************************************************************************/

#include "xttcps.h"
#include "xscugic.h"
#include "../gpio/ps7_gpio_if.h"


/*****************************************************************************/
/************************** Constant Definitions *****************************/
/*****************************************************************************/

#define TTC0_DEBUG						0

/* PS7 TTC_0 (3 timers, 0,1,2) */
#define PS7_TTC0_DEVICE_ID			XPAR_PS7_TTC_0_DEVICE_ID
#define PS7_TTC1_DEVICE_ID			XPAR_PS7_TTC_1_DEVICE_ID
#define PS7_TTC2_DEVICE_ID			XPAR_PS7_TTC_2_DEVICE_ID

/* PS7 TTC_1 (3 timers, 3,4,5) */
#define PS7_TTC3_DEVICE_ID			XPAR_PS7_TTC_3_DEVICE_ID
#define PS7_TTC4_DEVICE_ID			XPAR_PS7_TTC_4_DEVICE_ID
#define PS7_TTC5_DEVICE_ID			XPAR_PS7_TTC_5_DEVICE_ID


/* Counter match values */
/* TTC Clock = 111MHz => Period = 9ns*/
/* Prescaler disabled; resolution = 9ns
 * Note this limits the max time to ~589.8us */
#define PRESCALER_VALUE			XTTCPS_CLK_CNTRL_PS_DISABLE

#define INTERVAL_50US			5556U	// 50us (9ns x 5556)
#define INTERVAL_75US			8333U	// 75us (9ns x 8333)
#define INTERVAL_100US			11111U	// 100us (9ns x 11111)
#define INTERVAL_200US			22222U	// 200us (9ns x 22222)
#define INTERVAL_300US			33333U	// 300us (9ns x 33333)
#define INTERVAL_400US			44444U	// 400us (9ns x 44444)
#define INTERVAL_500US			55555U	// 500us (9ns x 55555)
#define INTERVAL_580US			64444U	// 580us (9ns x 64444)

#define INTERVAL_373US			41444U	// 400us (9ns x 41444)


// Interrupt settings
#define TTC0_0_INT_ID		XPS_TTC0_0_INT_ID		// 42U
#define TTC0_1_INT_ID		XPS_TTC0_1_INT_ID		// 43U
#define TTC0_2_INT_ID		XPS_TTC0_2_INT_ID 		// 44U
#define TTC1_0_INT_ID		XPS_TTC1_0_INT_ID		// 69U
#define TTC1_1_INT_ID		XPS_TTC1_1_INT_ID		// 70U
#define TTC1_2_INT_ID		XPS_TTC1_2_INT_ID		// 71U

// Common priority/trigger for all.
#define TTC_INTR_PRI		(0xB0)
#define TTC_INTR_TRIG		(0x01) // Active-high Level Sensitive


/*****************************************************************************/
/******************************* Typedefs ************************************/
/*****************************************************************************/



/*****************************************************************************/
/************************** Variable Declarations ****************************/
/*****************************************************************************/


/****************************************************************************/
/***************** Macros (Inline Functions) Definitions ********************/
/****************************************************************************/


/*****************************************************************************/
/************************** Function Prototypes ******************************/
/*****************************************************************************/

/* Device Initialisation */
int xTtcInit(uint16_t ttc_id,
				XTtcPs* p_XTtcPsInst,
				XScuGic* p_xInterruptController,
				Xil_ExceptionHandler fp_IntrHandler);



/* Interface functions */
void startTtc(XTtcPs* p_XTtcPsInst);
void resetTtc(XTtcPs* p_XTtcPsInst);




#endif /* SRC_TIMERS_TTC0_IF_H_ */
