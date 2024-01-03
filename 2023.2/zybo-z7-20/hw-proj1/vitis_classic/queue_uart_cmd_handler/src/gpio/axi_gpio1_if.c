 /******************************************************************************
 * @Title		:	AXI GPIO Interface (Header File)
 * @Filename	:	axi_gpio1_if.h
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



/******************************************************************************
* AXI GPIO1 Channel and Pin Mapping
*
* Channel 1 = outputs: PMOD JD = LDS[7:0], PMOD JE pins 7-10
*	Mapping is:
*		Bit 0-7: LD0 - LD7. (PMOD JD pins 1-4, 7-10)
*		Bit 8-11: PMOD_JE_7, PMOD_JE_8, PMOD_JE_9, PMOD_JE_10;
*
* Channel 2 = inputs: Board BTN[3:2], SW[3:2].
*	Mapping is:
*		Bit 0:BTN2; Bit1:BTN3;
* 		Bit 2:SW2; Bit 3:SW3;
*
******************************************************************************/


/*****************************************************************************/
/***************************** Include Files *********************************/
/*****************************************************************************/

#include "axi_gpio1_if.h"



/*****************************************************************************/
/************************** Variable Declarations ****************************/
/*****************************************************************************/

/* Declare instance and associated pointer for AXI GPIO */
static XGpio 		XGpio1Inst;
static XGpio 		*p_XGpio1Inst = &XGpio1Inst;




/*---------------------------------------------------------------------------*/
/*------------------------------- FUNCTIONS ---------------------------------*/
/*---------------------------------------------------------------------------*/


/*****************************************************************************
 * Function: AxiGpio1Init()
 *//**
 *
 * @brief		Configures the PL AXI GPIO for use.
 *
 *
 * @details		Starts by doing device look-up, configuration and self-test.
 * 				Then configures the	channels. In this project, channel 1 is the
 * 				output channel,	and channel 2 is the input channel.
 *
 * 				The initialisation steps are:
 * 				(1) DEVICE LOOK-UP => Calls function "XGpio_LookupConfig"
 * 				(2) DRIVER INIT => Calls function "XGpio_CfgInitialize"
 * 				(3) SELF TEST => Calls function "XGpio_SelfTest"
 * 				(4) SPECIFIC CONFIG => Configures GPIO channels and pins
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

int axiGpio1Init(void)
{
	int status;

	/* Pointer to XGpio_Config is required for later functions. */
	XGpio_Config *p_XGpio1Cfg = NULL;



	/* === START CONFIGURATION SEQUENCE ===  */

	/* ---------------------------------------------------------------------
	 * ------------ STEP 1: DEVICE LOOK-UP ------------
	 * -------------------------------------------------------------------- */
	p_XGpio1Cfg = XGpio_LookupConfig(AXI_GPIO1_DEVICE_ID);
	 if (p_XGpio1Cfg == NULL)
	 {
		 status = XST_FAILURE;
		 return status;
	 }


	 /* ---------------------------------------------------------------------
	  * ------------ STEP 2: DRIVER INITIALISATION ------------
	  * -------------------------------------------------------------------- */
	 status = XGpio_CfgInitialize(p_XGpio1Inst, p_XGpio1Cfg, p_XGpio1Cfg->BaseAddress);
	 if (status != XST_SUCCESS)
	 {
		 return status;
	 }


	/* ---------------------------------------------------------------------
	* ------------ STEP 3: SELF TEST ------------
	* -------------------------------------------------------------------- */
	status = XGpio_SelfTest(p_XGpio1Inst);
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
	/* Configure channel 1 to be all outputs */
	XGpio_SetDataDirection(p_XGpio1Inst, AXI_GPIO1_OP_CHANNEL, 0);

	/* Configure channel 2 to be inputs, depending on AXI_GPIO1_IP_MASK */
	XGpio_SetDataDirection(p_XGpio1Inst, AXI_GPIO1_IP_CHANNEL, AXI_GPIO1_IP_MASK);

	/* All outputs initially low */
	axiGp1_OutClear(LD0);
	axiGp1_OutClear(LD1);
	axiGp1_OutClear(LD2);
	axiGp1_OutClear(LD3);
	axiGp1_OutClear(LD4);
	axiGp1_OutClear(LD5);
	axiGp1_OutClear(LD6);
	axiGp1_OutClear(LD7);
	axiGp1_OutClear(PMOD_JE_7);
	axiGp1_OutClear(PMOD_JE_8);
	axiGp1_OutClear(PMOD_JE_9);
	axiGp1_OutClear(PMOD_JE_10);
	/* === END CONFIGURATION SEQUENCE ===  */



	/* Return initialisation result to calling code */
	return status;

}




/*****************************************************************************
 * Function: axiGp1_OutSet()
 *//**
 *
 * @brief		Sets an AXI GPIO output pin.
 *
 * @param[in]	AxiGpio1_OutPin_t pin: The AXI channel(1) pin to set.
 *
 * @return		None
 *
 * @note		Assert functionality: Only accept output channel bits [11:0];
 * 				Assert otherwise.
 *
******************************************************************************/

void axiGp1_OutSet(AxiGpio1_OutPin_t pin){

	/* Function should only be passed GPIO bits 0-11 */
	Xil_AssertVoid(pin < 12);

	XGpio_DiscreteSet(p_XGpio1Inst, AXI_GPIO1_OP_CHANNEL, (1 << pin));

}



/*****************************************************************************
 * Function: axiGp1_OutClear()
 *//**
 *
 * @brief		Clears an AXI GPIO output pin.
 *
 * @param[in]	AxiGpio1_OutPin_t pin		The AXI channel(1) pin to clear.
 *
 * @return		None
 *
 * @note		Assert functionality: Only accept output channel bits [11:0];
 * 				Assert otherwise..
 *
******************************************************************************/

void axiGp1_OutClear(AxiGpio1_OutPin_t pin){

	/* Function should only be passed GPIO bits 0-11 */
	Xil_AssertVoid(pin < 12);

	XGpio_DiscreteClear(p_XGpio1Inst, AXI_GPIO1_OP_CHANNEL, (1 << pin));

}



/*****************************************************************************
 * Function: axiGp1_OutToggle()
*//**
 *
 * @brief		Toggles an AXI GPIO output pin.
 *
 * @param[in]	AxiGpio1_OutPin_t pin		The AXI channel(1) pin to toggle.
 *
 * @return		None
 *
 * @note		Assert functionality: Only accept output channel bits [11:0];
 * 				Assert otherwise.
 *
******************************************************************************/

void axiGp1_OutToggle(AxiGpio1_OutPin_t pin){

	uint32_t pin_state;

	/* Function should only be passed GPIO bits 0-11 */
	Xil_AssertVoid(pin < 12);


	/* Read the current pin state, then use it to change the pin accordingly */
	pin_state = (XGpio_DiscreteRead(p_XGpio1Inst, AXI_GPIO1_OP_CHANNEL)
	 				& (1 << pin)) != 0;

	if (pin_state == 1)
	{
		axiGp1_OutClear(pin);
	}
	else
	{
		axiGp1_OutSet(pin);
	}
}



/*****************************************************************************
 * Function: axiGp1_InRead()
 *//**
 *
 * @brief		Reads an AXI GPIO input pin.
 *
 * @param[in]	AxiGpio1_OutPin_t pin		The AXI channel(2) pin to read.
 *
 * @return		1U if pin is set, else 0U.
 *
 * @note		Assert functionality: Only accept input channel bits [11:0];
 * 				Assert otherwise.
 *
******************************************************************************/

uint32_t axiGp1_InRead(AxiGpio1_InPin_t pin){

	uint32_t pin_state;

	/* Function should only be passed GPIO bits 0-3 */
	Xil_AssertNonvoid(pin < 4);

	/* Read input channel and check if bit is set */
	pin_state = (XGpio_DiscreteRead(p_XGpio1Inst, AXI_GPIO1_IP_CHANNEL)
					& (1 << pin)) != 0;

	return pin_state;

}


/****** End functions *****/

/****** End of File **********************************************************/






