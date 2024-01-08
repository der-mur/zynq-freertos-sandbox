/******************************************************************************
 * @Title		:	Semaphore Uart Command Handler
 * @Filename	:	semaphore_uart_cmd_handler.c
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

/*****************************************************************************/
/***************************** Include Files *********************************/
/*****************************************************************************/


/* FreeRTOS includes. */
#include "simple_tasks_example1.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "semphr.h"

/* Xilinx includes. */

/* User includes. */
#include "gpio/ps7_gpio_if.h"
#include "gpio/axi_gpio0_if.h"
#include "gpio/axi_gpio1_if.h"


/*****************************************************************************/
/***************************** Task Details **********************************/
/*****************************************************************************/

/* -----------------Task Priorities --------------------------- */

#define SIMPLE_TASK1_PRI				1
#define SIMPLE_TASK2_PRI				1



/* ----------------------- Tasks --------------------------- */
static void vSimpleTask1( void *pvParameters ); // Toggle LED 1
static void vSimpleTask2( void *pvParameters ); // Toggle LED 2
static TaskHandle_t SimpleTask1_handle;
static TaskHandle_t SimpleTask2_handle;
/* End Tasks defs */






/*****************************************************************************/
/************************** Function Prototypes ******************************/
/*****************************************************************************/

/* Init drivers. */
void vLowLevelSysInit(void);

/* The Idle task hook. */
void vApplicationIdleHook(void);



/***************************************************************************/



/*============================================*/
/* ================= MAIN CODE ===============*/
/*============================================*/

int main( void )
{

	/* ---------------------------------------------------- */
	/* Low-level init: SCUGIC, UART, all GPIOs -------------*/
	/* ---------------------------------------------------- */

	vLowLevelSysInit();



	/* ---------------------------------------------------- */
	/* ------ Create the tasks/semaphores/queues. --------- */
	/* ---------------------------------------------------- */

	/* Task 1 is low priority task that simply toggles LED 1. */
	xTaskCreate( 	vSimpleTask1, 							/* The function that implements the task. */
					( const char * ) "Task 1", 				/* Text name for the task, provided to assist debugging only. */
					configMINIMAL_STACK_SIZE, 				/* The stack allocated to the task. */
					NULL, 									/* The task parameter is not used, so set to NULL. */
					tskIDLE_PRIORITY + SIMPLE_TASK1_PRI,	/* Set Priority. */
					&SimpleTask1_handle );


	/* Task 2 is low priority task that simply toggles LED 1. */
	xTaskCreate( 	vSimpleTask2, 							/* The function that implements the task. */
					( const char * ) "Task 2", 				/* Text name for the task, provided to assist debugging only. */
					configMINIMAL_STACK_SIZE, 				/* The stack allocated to the task. */
					NULL, 									/* The task parameter is not used, so set to NULL. */
					tskIDLE_PRIORITY + SIMPLE_TASK2_PRI,	/* Set Priority. */
					&SimpleTask2_handle );









	/* ---------------------------------------------------- */

	/* start the scheduler. */
	vTaskStartScheduler();



	/* If all is well, the scheduler will now be running, and the following line
	will never be reached. */

	while (1);

}

/* --- END MAIN ---------------------------------------------------------*/




/*============================================*/
/* ================== TASKS ==================*/
/*============================================*/


/*****************************************************************************
 * Function: vSimpleTask1( void *pvParameters )
 *//**
 *
 * @brief	Runs every 10ms. Simply toggles LED 1.
 *
******************************************************************************/

static void vSimpleTask1( void *pvParameters )
{

	// Keep track of LED count so that we can slow down LED toggle rate:
	static volatile uint32_t led1_count = 0;


	// 10ms task timing
	TickType_t xPreviousWakeTime = 0;
	const TickType_t xPeriod = pdMS_TO_TICKS( DELAY_10_MSEC );

	xPreviousWakeTime = xTaskGetTickCount();

	/* Keep track of Task 2 suspended state */
	static uint32_t task2_suspended = 0;

	while(1)
	{
		psGpOutSet(PS_GP_OUT1); // TEST SIGNAL: Task Entered (Delayed)

		// Delay
		vTaskDelayUntil(&xPreviousWakeTime, xPeriod);

		psGpOutClear(PS_GP_OUT1); // TEST SIGNAL: End of delay
		psGpOutSet(PS_GP_OUT2); // TEST SIGNAL: Start of task

		// Toggle LED
		led1_count++;
		if (led1_count >= LED1_TOGGLE_COUNT)
		{
			axiGp0_OutToggle(LED1);
			led1_count = 0;
		}


		// Dummy delay so that GPIO signal can be clearly seen on scope.
		for (int i = 5000; i >=0; i--) {}


		/* Suspend Task 2 if SW0 is on. */
		if ( (task2_suspended == 0) && (axiGp0_InRead(SW0) == 1) )
		{
			vTaskSuspend(SimpleTask2_handle);
			task2_suspended = 1;
		}
		else if ( (task2_suspended == 1) && (axiGp0_InRead(SW0) == 0) )
		{
			task2_suspended = 0;
			vTaskResume(SimpleTask2_handle);
		}


		psGpOutClear(PS_GP_OUT2); // TEST SIGNAL: End of task
	}
}


/*****************************************************************************
 * Function: vSimpleTask2( void *pvParameters )
 *//**
 *
 * @brief	Runs every 10ms. Simply toggles LED 2.
 *
******************************************************************************/

static void vSimpleTask2( void *pvParameters )
{

	// Keep track of LED count so that we can slow down LED toggle rate:
	static volatile uint32_t led2_count = 0;


	// 10ms task timing
	TickType_t xPreviousWakeTime = 0;
	const TickType_t xPeriod = pdMS_TO_TICKS( DELAY_20_MSEC );

	xPreviousWakeTime = xTaskGetTickCount();

	while(1)
	{

		psGpOutSet(PS_GP_OUT3); // TEST SIGNAL: Task Entered (Delayed)

		// Delay
		vTaskDelayUntil(&xPreviousWakeTime, xPeriod);

		psGpOutClear(PS_GP_OUT3); // TEST SIGNAL: End of delay
		psGpOutSet(PS_GP_OUT4); // TEST SIGNAL: Start of task

		// Toggle LED
		led2_count++;
		if (led2_count >= LED2_TOGGLE_COUNT)
		{
			axiGp0_OutToggle(LED2);
			led2_count = 0;
		}

		// Dummy delay so that GPIO signal can be clearly seen on scope.
		for (int i = 5000; i >=0; i--) {}
		psGpOutClear(PS_GP_OUT4); // TEST SIGNAL: End of task

	}

}



void vApplicationIdleHook(void)
{

	psGpOutSet(PS_GP_OUT0); // TEST SIGNAL: Start of Idle Task

	// Dummy delay so that GPIO signal can be clearly seen on scope.
	for (int i = 500; i >=0; i--) {}

	psGpOutClear(PS_GP_OUT0); // TEST SIGNAL: End of Idle Task
}









/* --- END TASKS ---------------------------------------------------------*/



/*============================================*/
/* ======== MISCELLANEOUS FUNCTIONS ==========*/
/*============================================*/


/*****************************************************************************
 * Function: vLowLevelSysInit()
 *//**
 *
 * @brief	Calls a range of lower-level interface functions to initialise
 * 			the device drivers (GIC, GPIO, UART, TTC, etc.).
 *
******************************************************************************/

void vLowLevelSysInit(void)
{


	/* Keep track of initialisation using this struct */
	LowLevelInitStatus_s LowLevelInitStatus = {	.xgpio0 = XST_FAILURE,
												.xgpio1 = XST_FAILURE,
												.xgpiops = XST_FAILURE
	};


	/* ===== Initialization =====
	 * (3) AXI GPIO 0
	 * (4) AXI GPIO 1
	 * (5) AXI PS7 GPIO
	 */

#if PRINT_INIT_STATUS_TO_TERMINAL
	printf("\n\r===== Initializing Drivers =====\n\r");
#endif

	LowLevelInitStatus.xgpio0 = axiGpio0Init();
	LowLevelInitStatus.xgpio1 = axiGpio1Init();
	LowLevelInitStatus.xgpiops = psGpioInit();



/* Print results to console */
#if PRINT_INIT_STATUS_TO_TERMINAL

	printf("AXI GPIO 0 initialization: ");
	if (LowLevelInitStatus.xgpio0 != XST_SUCCESS) 		{ printf("Error detected.\n\r"); }
	else												{ printf("Success.\n\r"); }

	printf("AXI GPIO 1 initialization: ");
	if (LowLevelInitStatus.xgpio1 != XST_SUCCESS) 		{ printf("Error detected.\n\r"); }
	else												{ printf("Success.\n\r"); }

	printf("PS7 GPIO initialization: ");
	if (LowLevelInitStatus.xgpiops != XST_SUCCESS) 		{ printf("Error detected.\n\r\n\r"); }
	else												{ printf("Success.\n\r\n\r"); }

#endif




	/* --- CHECK INITIALISATION STATUS ---*/

	int init_status = ( (LowLevelInitStatus.xgpio0 == XST_SUCCESS) &&
						(LowLevelInitStatus.xgpio1 == XST_SUCCESS) &&
						(LowLevelInitStatus.xgpiops == XST_SUCCESS)
						);



	 /* If initialisation is successful, LED0 is turned on to indicate that the system
	  * is ready. If initialisation is not successful, then LED0 will flash, and the
	  * program will not continue to the main phase. */

	if ( init_status == 1) {
		axiGp0_OutSet(LED0);
		#if PRINT_INIT_STATUS_TO_TERMINAL
			printf("\n\rSystem ready: LED0 should be on.\n\r");
		#endif
	}
	else {
		#if PRINT_INIT_STATUS_TO_TERMINAL
			printf("\n\r!!! INITIALIZATION FAILED !!!\n\r");
		#endif
		while(1) { // Stay in this loop

		/* Toggle LED to show init failed. */
		axiGp0_OutToggle(LED0);

		uint32_t delay = 0U;
		for (delay = 0; delay < INIT_FAIL_LOOP_DELAY; delay++)
			{}
		}
	}
}

