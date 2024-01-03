/******************************************************************************
 * @Title		:	Semaphore Uart Command Handler
 * @Filename	:	semaphore_uart_cmd_handler.h
 * @Author		:	Derek Murray
 * @Origin Date	:	02/01/2024
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

#ifndef _SEMAPHORE_UART_CMD_HANDLER_H_
#define _SEMAPHORE_UART_CMD_HANDLER_H_


// Print info:
#define PRINT_INIT_STATUS_TO_TERMINAL		1


/*****************************************************************************/
/***************************** Include Files *********************************/
/*****************************************************************************/

#include "FreeRTOS.h"


/* ------------------------- RUN-TIME PARAMETERS ---------------------- */

/* Toggle rate for LED0 if initialisation fails */
#define INIT_FAIL_LOOP_DELAY		10000000U


// Use these to slow down the LED toggle rate to make it visible:
#define LED1_TOGGLE_COUNT			10U
#define LED2_TOGGLE_COUNT			1000U
#define LED3_TOGGLE_COUNT			1000U



/*****************************************************************************/
/******************************* Typedefs ************************************/
/*****************************************************************************/

/* Typedef to keep track of low-level initialisation progress */
typedef struct {
	volatile int xgpio0;
	volatile int xgpio1;
	volatile int xgpiops;
	volatile int xscu_gic;
	volatile int uart1;
} LowLevelInitStatus_s;



/*****************************************************************************/
/******************************* CONSTANSTS **********************************/
/*****************************************************************************/


#define DELAY_100_SECONDS	100000UL
#define DELAY_20_SECONDS	20000UL
#define DELAY_10_SECONDS	10000UL
#define DELAY_1_SECOND		1000UL
#define DELAY_200_MSEC		200UL
#define DELAY_100_MSEC		100UL
#define DELAY_30_MSEC		30UL
#define DELAY_20_MSEC		20UL
#define DELAY_18_MSEC		18UL
#define DELAY_10_MSEC		10UL
#define DELAY_8_MSEC		8UL
#define DELAY_1_MSEC		1UL
#define DELAY_3_MSEC		3UL
#define DELAY_5_MSEC		5UL


const TickType_t x100seconds = pdMS_TO_TICKS( DELAY_100_SECONDS );
const TickType_t x20seconds = pdMS_TO_TICKS( DELAY_20_SECONDS );
const TickType_t x10seconds = pdMS_TO_TICKS( DELAY_10_SECONDS );
const TickType_t x1second = pdMS_TO_TICKS( DELAY_1_SECOND );
const TickType_t x200ms = pdMS_TO_TICKS( DELAY_200_MSEC );
const TickType_t x100ms = pdMS_TO_TICKS( DELAY_100_MSEC );
const TickType_t x30ms = pdMS_TO_TICKS( DELAY_30_MSEC );
const TickType_t x20ms = pdMS_TO_TICKS( DELAY_20_MSEC );
const TickType_t x18ms = pdMS_TO_TICKS( DELAY_18_MSEC );
const TickType_t x10ms = pdMS_TO_TICKS( DELAY_10_MSEC );
const TickType_t x8ms = pdMS_TO_TICKS( DELAY_8_MSEC );
const TickType_t x5ms = pdMS_TO_TICKS( DELAY_5_MSEC );
const TickType_t x3ms = pdMS_TO_TICKS( DELAY_3_MSEC );
const TickType_t x1ms = pdMS_TO_TICKS( DELAY_1_MSEC );



#endif // _SEMAPHORE_UART_CMD_HANDLER_H_
