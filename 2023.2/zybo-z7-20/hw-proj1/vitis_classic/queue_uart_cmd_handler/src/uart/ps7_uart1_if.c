/******************************************************************************
 * @Title		:	UART1 Interface
 * @Filename	:	ps7_uart1_if.c
 * @Author		:	Derek Murray
 * @Origin Date	:	28/12/2023
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


/*****************************************************************************/
/***************************** Include Files *********************************/
/*****************************************************************************/

#include "ps7_uart1_if.h"



/*****************************************************************************/
/************************** Function Prototypes ******************************/
/*****************************************************************************/

// Interrupt handler, defined in main code:
extern void vUartIntrHandlerForQueue(void *CallBackRef, uint32_t event, uint32_t event_data);


/*---------------------------------------------------------------------------*/
/*------------------------------- FUNCTIONS ---------------------------------*/
/*---------------------------------------------------------------------------*/


/*****************************************************************************
 * Function: xUart1PsInit()
 *//**
 *
 * @brief		Configures UART1 for use.
 *
 *
 * @details		Starts by doing device look-up, configuration and self-test.
 * 				Then configures the	TTC0 Timer for this project.
 *
 * 				The initialisation steps are:
 * 				(1) DEVICE LOOK-UP => Calls function "XUartPs_LookupConfig"
 * 				(2) DRIVER INIT => Calls function "XUartPs_CfgInitialize"
 * 				(3) SELF TEST => Calls function "XUartPs_SelfTest"
 * 				(4) SPECIFIC CONFIG => Configures UART1 for this project
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
 * @note		p_XUartPsInst and  p_xInterruptController must be passed by
 * 				reference from calling code.
 *
******************************************************************************/

int xUartPs1Init(XUartPs* p_XUartPsInst, XScuGic* p_xInterruptController)
{
	int status;

	/* Pointer to XUartPs_Config is required for later functions. */
	XUartPs_Config *p_XUartPsCfg = NULL;


	/* === START CONFIGURATION SEQUENCE ===  */

	/* ---------------------------------------------------------------------
	 * ------------ STEP 1: DEVICE LOOK-UP ------------
	 * -------------------------------------------------------------------- */
	p_XUartPsCfg = XUartPs_LookupConfig(XPAR_PS7_UART_1_DEVICE_ID);
 	if (p_XUartPsCfg == NULL)
	{
 		status = XST_FAILURE;
 		return status;
	}


 	/* ---------------------------------------------------------------------
	 * ------------ STEP 2: DRIVER INITIALISATION ------------
	 * -------------------------------------------------------------------- */
	status = XUartPs_CfgInitialize(p_XUartPsInst, p_XUartPsCfg, p_XUartPsCfg->BaseAddress);
	if (status != XST_SUCCESS)
	{
 		return status;
	}


	/* ---------------------------------------------------------------------
	* ------------ STEP 3: SELF TEST ------------
	* -------------------------------------------------------------------- */
 	status = XUartPs_SelfTest(p_XUartPsInst);
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
	 * (1) Set the interrupt handler.
	 * (2) Enable desired interrupts. ('Receiver Full' is all that is needed.)
	 * (3) Configure the UART in Normal Mode.*/

	XUartPs_SetHandler(p_XUartPsInst, (XUartPs_Handler)vUartIntrHandlerForQueue, p_XUartPsInst);
	XUartPs_SetInterruptMask(p_XUartPsInst, XUARTPS_IXR_RXFULL );
	XUartPs_SetOperMode(p_XUartPsInst, XUARTPS_OPER_MODE_NORMAL);



	/* ---------------------------------------------------------------------
	* ------------ STEP 5: INTERRUPT CONFIGURATION ------------
	* -------------------------------------------------------------------- */

	/* Interrupt configuration */
	/* Connect a device driver handler for Uart1 */
	status = XScuGic_Connect(p_xInterruptController, UART1_INTR_ID,
				  (Xil_ExceptionHandler) XUartPs_InterruptHandler,
				  (void *) p_XUartPsInst);
	if (status != XST_SUCCESS)
	{
		return XST_FAILURE;
	}


	/* Set priority and trigger type */
	XScuGic_SetPriorityTriggerType(p_xInterruptController,
									UART1_INTR_ID,
									UART1_INTR_PRI,
									UART1_INTR_TRIG);


	/* Enable the interrupt for Uart1 */
	XScuGic_Enable(p_xInterruptController, UART1_INTR_ID);

	/* Return initialisation result to calling code */
	return status;

}



/****** End functions *****/

/****** End of File **********************************************************/

