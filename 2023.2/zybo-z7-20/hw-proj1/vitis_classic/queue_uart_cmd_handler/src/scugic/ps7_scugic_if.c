/******************************************************************************
 * @Title		:	PS7 SCUGIC Interface Source File
 * @Filename	:	ps7_scugic_if.c
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


/*****************************************************************************/
/***************************** Include Files *********************************/
/*****************************************************************************/

#include "ps7_scugic_if.h"




/*---------------------------------------------------------------------------*/
/*------------------------------- FUNCTIONS ---------------------------------*/
/*---------------------------------------------------------------------------*/



/*****************************************************************************
 * Function: xScuGic_Init(XScuGic* p_XScuGicInst)
 *//**
 *
 * @brief		Configures the Global Interface Controller (GIC) for use.
 *
 *
 * @details		Starts by doing device look-up, configuration and self-test.
 * 				Then configures the	SCUGIC.
 *
 * 				The initialisation steps are:
 * 				(1) DEVICE LOOK-UP => Calls function "XScuGic_LookupConfig"
 * 				(2) DRIVER INIT => Calls function "XScuGic_CfgInitialize"
 * 				(3) SELF TEST => Calls function "XScuGic_SelfTest"
 * 				(4) SPECIFIC CONFIG => Configures SCUGIC for this project
 *
 * 				If any of the first three states results in XST_FAILURE, the
 * 				initialisation will stop and the XST_FAILURE code will be
 * 				returned to the calling code. If initialisation completes with
 * 				no failures, then XST_SUCCESS is returned.
 *
 * @return		Integer indicating result of configuration attempt.
 * 				0 = SUCCESS, 1 = FAILURE
 *
 * @note		p_XScuGicInst must be passed by reference from calling code.
 *
******************************************************************************/

int xScuGicInit(XScuGic* p_XScuGicInst){

	int status;


	/* Pointer to XScuGic_Config is required for later functions. */
	XScuGic_Config *p_XScuGicCfg = NULL;



	/* === START CONFIGURATION SEQUENCE ===  */

	/* ---------------------------------------------------------------------
	 * ------------ STEP 1: DEVICE LOOK-UP ------------
	 * -------------------------------------------------------------------- */
	p_XScuGicCfg = XScuGic_LookupConfig(PS7_SCUGIC_DEVICE_ID);
	if (p_XScuGicCfg == NULL)
	{
		status = XST_FAILURE;
		return status;
	}


	/* ---------------------------------------------------------------------
	 * ------------ STEP 2: DRIVER INITIALISATION ------------
	 * -------------------------------------------------------------------- */
	status = XScuGic_CfgInitialize(p_XScuGicInst, p_XScuGicCfg, p_XScuGicCfg->CpuBaseAddress);
	if (status != XST_SUCCESS)
	{
		return status;
	}


	/* ---------------------------------------------------------------------
	* ------------ STEP 3: SELF TEST ------------
	* -------------------------------------------------------------------- */
	status = XScuGic_SelfTest(p_XScuGicInst);
 	Xil_AssertNonvoid(status == XST_SUCCESS);

 	/* If the assertion test fails, we won't get here, but
 	 * leave the code in anyway, for possible future changes. */
 	if (status != XST_SUCCESS)
 	{
		 return status;
 	}


	/* ---------------------------------------------------------------------
	* ------------ STEP 4: PROJECT-SPECIFIC CONFIGURATION ------------
	* -------------------------------------------------------------------- */
 	// Initialise exception logic:
 	Xil_ExceptionInit();

	/*
	* Connect the interrupt controller interrupt handler to the
	* hardware interrupt handling logic in the processor.
	*/
	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
								(Xil_ExceptionHandler) XScuGic_InterruptHandler,
								p_XScuGicInst);



	/* === END CONFIGURATION SEQUENCE ===  */



	/* Return initialisation result to calling code */
	return status;

}



/*****************************************************************************
 * Function:	enableInterrupts()
 *//**
*
* @brief		Calls the Xilinx function "Xil_ExceptionEnable" to enable
* 				interrupts.
*
* @return		None.
*
* @notes:		None.
*
****************************************************************************/

void enableInterrupts(void){
	Xil_ExceptionEnable();
}



/*****************************************************************************
 * Function:	disableInterrupts()
 *//**
*
* @brief		Calls the Xilinx function "Xil_ExceptionDisable" to disable
* 				interrupts.
*
* @return		None.
*
* @notes:		None.
*
****************************************************************************/

void disableInterrupts(void){
	Xil_ExceptionDisable();
}



/****** End functions *****/

/****** End of File **********************************************************/


