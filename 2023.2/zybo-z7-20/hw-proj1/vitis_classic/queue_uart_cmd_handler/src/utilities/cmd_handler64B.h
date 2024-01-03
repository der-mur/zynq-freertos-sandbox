/******************************************************************************
 * @Title		:	Command Handler (Header File)
 * @Filename	:	cmd_handler64B.h
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


#ifndef SRC_CMD_HANDLER_H_
#define SRC_CMD_HANDLER_H_


/*****************************************************************************/
/***************************** Include Files *********************************/
/*****************************************************************************/

/* Xilinx files */
#include "xil_types.h"
#include "xil_io.h"


/*****************************************************************************/
/************************** Constant Definitions *****************************/
/*****************************************************************************/


#define WRITE_OKAY			(0x01010101U)
#define CMD_ERROR			(0xEEAA5577U)





/*****************************************************************************/
/******************************* Typedefs ************************************/
/*****************************************************************************/


typedef struct {
	uint16_t cmd;		// Bytes 0-1
	uint16_t sz;		// Bytes 2-3

	uint32_t field1; 	// Bytes 4-7		start addr in memory operations

	uint32_t field2;	// Bytes 8-11		data 1
	uint32_t field3;	// Bytes 12-15		data 2
	uint32_t field4;	// Bytes 16-19		data 3
	uint32_t field5;	// Bytes 20-23		data 4
	uint32_t field6;	// Bytes 24-27		data 5
	uint32_t field7;	// Bytes 28-31		data 6
	uint32_t field8;	// Bytes 32-35		data 7
	uint32_t field9;	// Bytes 36-39		data 8
	uint32_t field10;	// Bytes 40-43		data 9
	uint32_t field11;	// Bytes 44-47		data 10
	uint32_t field12;	// Bytes 48-51		data 11
	uint32_t field13;	// Bytes 52-55		data 12
	uint32_t field14;	// Bytes 56-59		data 13
	uint32_t field15;	// Bytes 60-63		data 14
} cmd_frame;



/* -------- Commands -------- */
typedef enum
{
	WRITE_WORD = 0x00D3,
	READ_WORD = 0x00D4,
	WRITE_SEQUENTIAL = 0x00D5,
	READ_SEQUENTIAL = 0x00D6
}commands;



/*****************************************************************************/
/************************** Variable Declarations ****************************/
/*****************************************************************************/


/****************************************************************************/
/***************** Macros (Inline Functions) Definitions ********************/
/****************************************************************************/


/*****************************************************************************/
/************************** Function Prototypes ******************************/
/*****************************************************************************/

/* Main function to be used by e.g. comms block ISR, FreeRTOS task, etc. */
void handleCommand64B(uint8_t *rx_buffer, uint8_t *tx_buffer);


#endif /* SRC_CMD_HANDLER_H_ */
