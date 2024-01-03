/******************************************************************************
 * @Title		:	Command Handler
 * @Filename	:	cmd_handler64B.c
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



/*****************************************************************************/
/***************************** Include Files *********************************/
/*****************************************************************************/

#include "cmd_handler64B.h"




/* Array to store the write data values for sequential write command. */
static uint32_t write_resp[16] = {	WRITE_OKAY, WRITE_OKAY, WRITE_OKAY,
									WRITE_OKAY, WRITE_OKAY, WRITE_OKAY,
									WRITE_OKAY, WRITE_OKAY, WRITE_OKAY,
									WRITE_OKAY, WRITE_OKAY, WRITE_OKAY,
									WRITE_OKAY, WRITE_OKAY, WRITE_OKAY,
									WRITE_OKAY };


static uint32_t cmd_error_resp[16] = {	CMD_ERROR, CMD_ERROR, CMD_ERROR,
										CMD_ERROR, CMD_ERROR, CMD_ERROR,
										CMD_ERROR, CMD_ERROR, CMD_ERROR,
										CMD_ERROR, CMD_ERROR, CMD_ERROR,
										CMD_ERROR, CMD_ERROR, CMD_ERROR,
										CMD_ERROR };



/*****************************************************************************/
/************************** Variable Declarations ****************************/
/*****************************************************************************/

/* Store cmd and data fields of received data in this structure */
static cmd_frame		CmdFrameInst;
static cmd_frame 		*p_cmd_frame = &CmdFrameInst;



/****************************************************************************/
/************************** Function Prototypes *****************************/
/****************************************************************************/

/* Functions internal to the command handler */
static void decodeRxData(uint8_t *rx_buffer);
static void executeCommand(uint8_t *tx_buffer);
static void setResponseBytes(uint8_t *tx_buffer, uint32_t tx_data);



/*---------------------------------------------------------------------------*/
/*------------------------------- FUNCTIONS ---------------------------------*/
/*---------------------------------------------------------------------------*/



/******************************************************************************
*
* Function:		handleCommand()
*
* Description:	Main function for command handling. Effectively a wrapper around
* 				the local functions decodeRxData() and executeCommand().
*
* param[in]		*rx_buffer: Pointer to the receive buffer in the comms block.
* param[in]		*tx_buffer: Pointer to the transmit buffer in the comms block.
*
* Returns:		None.
*
* Notes:		This is the interface function that is called by external code
* 				so that command handling is carried out.
*
****************************************************************************/

void handleCommand64B(uint8_t *rx_buffer, uint8_t *tx_buffer)
{

	/* Decode the receive data and execute the command */
	decodeRxData(rx_buffer);
	executeCommand(tx_buffer);

}



/******************************************************************************
*
* Function:		decodeRxData
*
* Description:	Decodes the received data and converts it to a 'cmd_frame'
* 				structure. Updates 'CmdFrameInst' using the pointer *p_cmd_frame.
*
* param[in]		*rx_buffer: Pointer to the receive buffer in the comms block.
*
* Returns:		None.
*
* Notes:
*
****************************************************************************/

void decodeRxData(uint8_t *rx_buffer){

	uint32_t idx = 0;

	/* ------ Extract command, bytes 0-1 ------- */ // COMMAND
	for (idx = 0; idx < 2; idx++){
		p_cmd_frame->cmd = (p_cmd_frame->cmd << 8) | (rx_buffer[idx]);
	}

	/* ------ Extract size, bytes 2-3 ------- */ // E.G. NUMBER OF BYTES TO READ/WRITE
	for (idx = 2; idx < 4; idx++){
		p_cmd_frame->sz = (p_cmd_frame->sz << 8) | (rx_buffer[idx]);
	}

	/* ------- Extract field 1, bytes 4-7 ------ */ // ADDR IN MEM CMDS
	for (idx = 4; idx < 8; idx++){
		p_cmd_frame->field1 = (p_cmd_frame->field1 << 8) | (rx_buffer[idx]);
	}

	/* -------  Extract field 2, bytes 8-11 ----- */
	for (idx = 8; idx < 12; idx++){
		p_cmd_frame->field2 = (p_cmd_frame->field2 << 8) | (rx_buffer[idx]);
	}

	/* -------  Extract field 3, bytes 12-15 ----- */
	for (idx = 12; idx < 16; idx++){
		p_cmd_frame->field3 = (p_cmd_frame->field3 << 8) | (rx_buffer[idx]);
	}

	/* -------  Extract field 4, bytes 16-19 ----- */
	for (idx = 16; idx < 20; idx++){
		p_cmd_frame->field4 = (p_cmd_frame->field4 << 8) | (rx_buffer[idx]);
	}

	/* -------  Extract field 5, bytes 20-23 ----- */
	for (idx = 20; idx < 24; idx++){
		p_cmd_frame->field5 = (p_cmd_frame->field5 << 8) | (rx_buffer[idx]);
	}

	/* -------  Extract field 6, bytes 24-27 ----- */
	for (idx = 24; idx < 28; idx++){
		p_cmd_frame->field6 = (p_cmd_frame->field6 << 8) | (rx_buffer[idx]);
	}

	/* -------  Extract field 7, bytes 28-31 ----- */
	for (idx = 28; idx < 32; idx++){
		p_cmd_frame->field7 = (p_cmd_frame->field7 << 8) | (rx_buffer[idx]);
	}

	/* -------  Extract field 8, bytes 32-35 ----- */
	for (idx = 32; idx < 36; idx++){
		p_cmd_frame->field8 = (p_cmd_frame->field8 << 8) | (rx_buffer[idx]);
	}

	/* -------  Extract field 9, bytes 36-39 ----- */
	for (idx = 36; idx < 40; idx++){
		p_cmd_frame->field9 = (p_cmd_frame->field9 << 8) | (rx_buffer[idx]);
	}

	/* -------  Extract field 10, bytes 40-43 ----- */
	for (idx = 40; idx < 44; idx++){
		p_cmd_frame->field10 = (p_cmd_frame->field10 << 8) | (rx_buffer[idx]);
	}

	/* -------  Extract field 11, bytes 44-47 ----- */
	for (idx = 44; idx < 48; idx++){
		p_cmd_frame->field11 = (p_cmd_frame->field11 << 8) | (rx_buffer[idx]);
	}

	/* -------  Extract field 12, bytes 48-51 ----- */
	for (idx = 48; idx < 52; idx++){
		p_cmd_frame->field12 = (p_cmd_frame->field12 << 8) | (rx_buffer[idx]);
	}

	/* -------  Extract field 13, bytes 52-55 ----- */
	for (idx = 52; idx < 56; idx++){
		p_cmd_frame->field13 = (p_cmd_frame->field13 << 8) | (rx_buffer[idx]);
	}


	/* -------  Extract field 14, bytes 56-59 ----- */
	for (idx = 56; idx < 60; idx++){
		p_cmd_frame->field14 = (p_cmd_frame->field14 << 8) | (rx_buffer[idx]);
	}



	/* -------  Extract field 15, bytes 60-63 ----- */
	for (idx = 60; idx < 63; idx++){
		p_cmd_frame->field15 = (p_cmd_frame->field15 << 8) | (rx_buffer[idx]);
	}

}



/******************************************************************************
*
* Function:		executeCommand()
*
* Description:	Executes the received command and directly updates the comms
* 				block transmit buffer with the response data.
*
* param[in]		*tx_buffer: Pointer to the transmit buffer in the comms block.
*
* Returns:		None.
*
* Notes:		None.
*
****************************************************************************/


void executeCommand(uint8_t *tx_buffer) {

	/* Extract fields from the cmd_frame structure */
	uint16_t cmd = p_cmd_frame->cmd;
	uint16_t sz = p_cmd_frame->sz;

	uint32_t field1 = p_cmd_frame->field1;
	uint32_t field2 = p_cmd_frame->field2;
	//
	uint32_t field3 = p_cmd_frame->field3;
	uint32_t field4 = p_cmd_frame->field4;
	uint32_t field5 = p_cmd_frame->field5;
	uint32_t field6 = p_cmd_frame->field6;
	uint32_t field7 = p_cmd_frame->field7;
	uint32_t field8 = p_cmd_frame->field8;
	uint32_t field9 = p_cmd_frame->field9;
	uint32_t field10 = p_cmd_frame->field10;
	uint32_t field11 = p_cmd_frame->field11;
	uint32_t field12 = p_cmd_frame->field12;
	uint32_t field13 = p_cmd_frame->field13;
	uint32_t field14 = p_cmd_frame->field14;
	uint32_t field15 = p_cmd_frame->field15;

	/* Variable to store memory read data */
	uint32_t mem_read_data;

	/* Array to store the write data values for sequential write command. */
	uint32_t seq_wr_data_array[14] = {	field2, field3, field4, field5, field6,
										field7, field8, field9, field10, field11,
										field12, field13, field14, field15 };

	// Generic loop variable
	uint32_t idx;





	/* ----- Switch-Case to handle the packet ----- */

	switch(cmd) {

	// --------------------------------------------------------------------------------- //
	// WRITE_WORD: 32-bit write to memory location
	// Field 1 = address ; Field 2 = Data
	// --------------------------------------------------------------------------------- //
	case WRITE_WORD:
		/* Write the data and update the response buffer */
		Xil_Out32(field1, field2);
		setResponseBytes(tx_buffer, WRITE_OKAY);
		break;


	// --------------------------------------------------------------------------------- //
	// CMD = 0x00D4: 32-bit read from memory location
	// Field 1 = address
	// --------------------------------------------------------------------------------- //
	case READ_WORD:
		/* Read the data and update the response buffer */
		mem_read_data = Xil_In32(field1);
		setResponseBytes(tx_buffer, mem_read_data);
		break;



	// --------------------------------------------------------------------------------- //
	// CMD = 0x00D5: 32-bit write to memory locations
	// Field 1 = Start Address ; Field 16 = Number of locations to write
	// --------------------------------------------------------------------------------- //
	case WRITE_SEQUENTIAL:
		// Write the data
		for (idx = 0; idx < sz; idx++)
		{
			Xil_Out32(field1, seq_wr_data_array[idx]);
			field1 += 4;
		}
		// Update the response buffer with response code (all 64 bytes)
		for (idx = 0; idx < 16; idx++)
		{
			setResponseBytes(tx_buffer,  write_resp[idx]);
			tx_buffer += 4;
		}

		break;

	// --------------------------------------------------------------------------------- //
	// CMD = 0x00D6: 32-bit read from memory locations
	// Field 1 = Start Address ; Field 16 = Number of locations to read
	// --------------------------------------------------------------------------------- //
	case READ_SEQUENTIAL:
		for (idx = 0; idx < sz; idx++)
		{
			mem_read_data = Xil_In32(field1);
			setResponseBytes(tx_buffer,  mem_read_data);
			field1 += 4;
			tx_buffer += 4;
		}
		break;


	// --------------------------------------------------------------------------------- //
	// Handle unknown commands
	// --------------------------------------------------------------------------------- //
	default:
		/* Update the response buffer with error code (all 64 bytes) */
		for (idx = 0; idx < 16; idx++)
		{
			setResponseBytes(tx_buffer,  cmd_error_resp[idx]);
			tx_buffer += 4;
		}
		// setResponseBytes(tx_buffer, CMD_ERROR);
		break;
	}

}




/******************************************************************************
*
* Function:		setResponseBytes
*
* Description:	Updates the 4-byte Tx Buffer of the communications block with
* 				the data to be transmitted back to the host.
*
* Returns:		None.
*
* Notes:		None.
*
****************************************************************************/

void setResponseBytes(uint8_t* tx_buffer, uint32_t tx_data)
{
	*tx_buffer = (tx_data >> 24) & 0xFF;
	*(tx_buffer + 1) = (tx_data >> 16) & 0xFF;
	*(tx_buffer + 2) = (tx_data >> 8) & 0xFF;
	*(tx_buffer + 3) = tx_data & 0xFF;
}



/****** End functions *****/

/****** End of File **********************************************************/
