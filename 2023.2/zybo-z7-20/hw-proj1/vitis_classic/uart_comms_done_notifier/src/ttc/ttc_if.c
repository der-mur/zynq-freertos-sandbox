/******************************************************************************
 * @Title		:	Triple Timer Counter Interface
 * @Filename	:	ttc0_if.c
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

#include "ttc_if.h"




/*****************************************************************************/
/************************** Constant Definitions *****************************/
/*****************************************************************************/



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




/*---------------------------------------------------------------------------*/
/*------------------------------- FUNCTIONS ---------------------------------*/
/*---------------------------------------------------------------------------*/



/*****************************************************************************
 * Function: xTtcInit()
 *//**
 *
 * @brief		Configures the TTC Timer for use.
 *
 *
 * @details		Starts by doing device look-up, configuration and self-test.
 * 				Then configures the	TTC0 Timer for this project.
 *
 * 				The initialisation steps are:
 * 				(1) DEVICE LOOK-UP => Calls function "XTtcPs_LookupConfig"
 * 				(2) DRIVER INIT => Calls function "XTtcPs_CfgInitialize"
 * 				(3) SELF TEST => Calls function "XTtcPs_SelfTest"
 * 				(4) SPECIFIC CONFIG => Configures TTC0 for this project
 * 				(5) Interrupt configuration.
 *
 * 				If any of the first three states results in XST_FAILURE, the
 * 				initialisation will stop and the XST_FAILURE code will be
 * 				returned to the calling code. If initialisation completes with
 * 				no failures, then XST_SUCCESS is returned.
 *
 * @return		Integer indicating result of configuration attempt.
 * 				0 = SUCCESS, 1 = FAILURE
 *
 * @note		None
 *
******************************************************************************/

int xTtcInit(uint16_t ttc_id,
				XTtcPs* p_XTtcPsInst,
				XScuGic* p_xInterruptController,
				Xil_ExceptionHandler fp_IntrHandler	)
{

	int status;


	uint16_t TTC_DEVICE_ID = 0;
	uint16_t INTERVAL = 0;

	// Set device id
	if (ttc_id == 0) { TTC_DEVICE_ID = PS7_TTC0_DEVICE_ID; }
	else if (ttc_id == 1) { TTC_DEVICE_ID = PS7_TTC1_DEVICE_ID; }
	else if (ttc_id == 2) { TTC_DEVICE_ID = PS7_TTC2_DEVICE_ID; }
	else if (ttc_id == 3) { TTC_DEVICE_ID = PS7_TTC3_DEVICE_ID; }
	else if (ttc_id == 4) { TTC_DEVICE_ID = PS7_TTC4_DEVICE_ID; }
	else if (ttc_id == 5) { TTC_DEVICE_ID = PS7_TTC5_DEVICE_ID; }


	// Set Interval (i.e. periodic interrupt time)
	if (ttc_id == 0) { INTERVAL = INTERVAL_75US; }
	else if (ttc_id == 1) { INTERVAL = INTERVAL_100US; }
	else if (ttc_id == 2) { INTERVAL = INTERVAL_500US; }
	else if (ttc_id == 3) { INTERVAL = INTERVAL_500US; }
	else if (ttc_id == 4) { INTERVAL = INTERVAL_500US; }
	else if (ttc_id == 5) { INTERVAL = INTERVAL_500US; }





	/* Pointer to XTtcPs_Config is required for later functions. */
	XTtcPs_Config *p_XTtc0PsCfg = NULL;


	/* === START CONFIGURATION SEQUENCE ===  */

	/* ---------------------------------------------------------------------
	 * ------------ STEP 1: DEVICE LOOK-UP ------------
	 * -------------------------------------------------------------------- */
	p_XTtc0PsCfg = XTtcPs_LookupConfig(TTC_DEVICE_ID);
 	if (p_XTtc0PsCfg == NULL)
	{
 		status = XST_FAILURE;
 		return status;
	}


 	/* ---------------------------------------------------------------------
	 * ------------ STEP 2: DRIVER INITIALISATION ------------
	 * -------------------------------------------------------------------- */
 	/*  TIMER MUST BE DISABLED BEFORE ATTEMPTING CONFIGURATION */
 	XTtcPs_WriteReg(p_XTtc0PsCfg->BaseAddress, XTTCPS_CNT_CNTRL_OFFSET, 1U);

 	status = XTtcPs_CfgInitialize(p_XTtcPsInst, p_XTtc0PsCfg, p_XTtc0PsCfg->BaseAddress);
 	if (status != XST_SUCCESS)
	{
 		return status;
	}


	/* ---------------------------------------------------------------------
	* ------------ STEP 3: SELF TEST ------------
	* -------------------------------------------------------------------- */
 	status = XTtcPs_SelfTest(p_XTtcPsInst);
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
	/* Configuration steps are:
	* (1) Set clock control options (prescaler)
	* (2) Set Interval mode.
	* (3) Set Interval time.
	* (4) Enable Interval interrupt.*/
	XTtcPs_SetPrescaler(p_XTtcPsInst, PRESCALER_VALUE);
	XTtcPs_SetOptions(p_XTtcPsInst, XTTCPS_OPTION_INTERVAL_MODE);
	XTtcPs_SetInterval(p_XTtcPsInst, INTERVAL);
	XTtcPs_EnableInterrupts(p_XTtcPsInst, XTTCPS_IXR_INTERVAL_MASK );



	/* ---------------------------------------------------------------------
	* ------------ STEP 5: INTERRUPT CONFIGURATION ------------
	* -------------------------------------------------------------------- */

	uint32_t TTC_INTR_ID = 0;

	// Set interrupt id
	if (ttc_id == 0) { TTC_INTR_ID = TTC0_0_INT_ID; }
	else if (ttc_id == 1) { TTC_INTR_ID = TTC0_1_INT_ID; }
	else if (ttc_id == 2) { TTC_INTR_ID = TTC0_2_INT_ID; }
	else if (ttc_id == 3) { TTC_INTR_ID = TTC1_0_INT_ID; }
	else if (ttc_id == 4) { TTC_INTR_ID = TTC1_1_INT_ID; }
	else if (ttc_id == 5) { TTC_INTR_ID = TTC1_2_INT_ID; }

	/* Interrupt configuration */
	/* Connect a device driver handler for Uart1 */
	status = XScuGic_Connect(p_xInterruptController, TTC_INTR_ID,
				  (Xil_ExceptionHandler) fp_IntrHandler,
				  (void *) p_XTtcPsInst);
	if (status != XST_SUCCESS)
	{
		return XST_FAILURE;
	}


	/* Set priority and trigger type */
	XScuGic_SetPriorityTriggerType(p_xInterruptController,
									TTC_INTR_ID,
									TTC_INTR_PRI,
									TTC_INTR_TRIG);


	/* Enable the interrupt for the TTC */
	XScuGic_Enable(p_xInterruptController, TTC_INTR_ID);


	/* Return initialisation result to calling code */
	return status;

}




/*****************************************************************************
 * Function: startTtc()
 *//**
 *
 * @brief		Starts TTC by calling XTtcPs_Start().
 *
 * @return 		None
 *
 * @note		None
 *
****************************************************************************/

void startTtc(XTtcPs* p_XTtcPsInst)
{
	XTtcPs_Start(p_XTtcPsInst);
}



/*****************************************************************************
 * Function: resetTtc()
 *//**
 *
 * @brief		Resets(and restarts) TTC by calling XTtcPs_ResetCounterValue().
 *
 * @return 		None
 *
 * @note		None
 *
****************************************************************************/

void resetTtc(XTtcPs* p_XTtcPsInst)
{
	XTtcPs_ResetCounterValue(p_XTtcPsInst);
}




/****** End functions *****/

/****** End of File **********************************************************/
