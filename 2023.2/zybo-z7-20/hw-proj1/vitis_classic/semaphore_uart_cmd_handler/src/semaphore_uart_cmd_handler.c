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
#include "semaphore_uart_cmd_handler.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "semphr.h"

/* Xilinx includes. */
#include "xuartps.h"
#include "xscugic.h"

/* User includes. */
#include "scugic/ps7_scugic_if.h"
#include "uart/ps7_uart1_if.h"
#include "gpio/ps7_gpio_if.h"
#include "gpio/axi_gpio0_if.h"
#include "gpio/axi_gpio1_if.h"


/*****************************************************************************/
/***************************** Task Details **********************************/
/*****************************************************************************/

/* -----------------Task Priorities --------------------------- */

#define BACKGROUND_TASK1_PRI				1
#define CMDHANDLER_TASK_PRI					2




/* ----------------------- Task 1 --------------------------- */
static void BackgroundTask1( void *pvParameters ); // Toggle LED 1
static TaskHandle_t BackgroundTask1_handle;
/* End Task 1 defs */




/* ------------ Command handler task and semaphore -------------- */

static void vCommandHandlerTask( void *pvParams);
static TaskHandle_t xCommandHandler_h;

static SemaphoreHandle_t xBinarySemaphore;




/* -------------------------- UART buffer details ------------------------ */

#define UART_RX_BUFFER_SIZE			64U		// 64 byte command frame from host
#define UART_TX_BUFFER_SIZE			64U		// 64 byte response frame to host

/* Uart Buffer for receiving data from host */
static uint8_t RxBuffer [UART_RX_BUFFER_SIZE] = {0};
/* Uart Buffer for sending data to host */
static uint8_t TxBuffer [UART_TX_BUFFER_SIZE] = {0};






/*****************************************************************************/
/*********************** Low-level Driver instances **************************/
/*****************************************************************************/

extern XScuGic xInterruptController;
static XUartPs xUartPs1Inst;
// Note: GPIO driver instances are created at lower level.



/*****************************************************************************/
/************************** Function Prototypes ******************************/
/*****************************************************************************/

/* Init drivers. */
void vLowLevelSysInit(void);

/* The Idle task hook. */
void vApplicationIdleHook(void);

void vUartIntrHandler(void *CallBackRef, uint32_t event, uint32_t event_data);

/* Defined in cmd_handler code. Used in Command Handler Rx task. */
extern void handleCommand64B(uint8_t *rx_buffer, uint8_t *tx_buffer);



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
	xTaskCreate( 	BackgroundTask1, 							/* The function that implements the task. */
					( const char * ) "Task 1", 				/* Text name for the task, provided to assist debugging only. */
					configMINIMAL_STACK_SIZE, 				/* The stack allocated to the task. */
					NULL, 									/* The task parameter is not used, so set to NULL. */
					tskIDLE_PRIORITY + BACKGROUND_TASK1_PRI,			/* Set Priority. */
					&BackgroundTask1_handle );






	/* Command handler Task and semaphore. The task waits on a semaphore sent by the PS7 UART interrupt handler.
	 *  The task has higher priority than Task 1 so that it can preempt Task 1 to receive UART data
	 *  as soon as the HW interrupt occurs.*/
	xBinarySemaphore = xSemaphoreCreateBinary();

	if(xBinarySemaphore != NULL) {
		xTaskCreate( vCommandHandlerTask,
					(const char*) "Command Handler",
					configMINIMAL_STACK_SIZE,
					NULL,
					tskIDLE_PRIORITY + CMDHANDLER_TASK_PRI,
					&xCommandHandler_h	);
	}


	/* ---------------------------------------------------- */



	/* Start the Zynq interrupt system before starting the scheduler.
	 * The interrupt system is used for low-level UART and TTC interrupts. */
	enableInterrupts();


	/* Finally, start the scheduler. */
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
 * Function: BackgroundTask1( void *pvParameters )
 *//**
 *
 * @brief	Runs in the background every 10ms. Simply toggles LED 1.
 *
******************************************************************************/

static void BackgroundTask1( void *pvParameters )
{

	// Keep track of LED count so that we can slow down LED toggle rate:
	static volatile uint32_t led1_count = 0;


	// 10ms task timing
	TickType_t xPreviousWakeTime = 0;
	const TickType_t xPeriod = pdMS_TO_TICKS( DELAY_10_MSEC );

	xPreviousWakeTime = xTaskGetTickCount();

	while(1)
	{

		psGpOutToggle(PS_GP_OUT1); // TEST SIGNAL: Toggle to indicate task is entered.

		// Delay
		vTaskDelayUntil(&xPreviousWakeTime, xPeriod);

		// Toggle LED
		led1_count++;
		if (led1_count >= LED1_TOGGLE_COUNT)
		{
			axiGp0_OutToggle(LED1);
			led1_count = 0;
		}
	}
}






/*****************************************************************************
 * Function: vCommandHandlerTask( void *pvParameters )
 *//**
 *
 * @brief	Command handler Task. Waits on a semaphore that is sent by the
 * 			UART interrupt handler when data (64 bytes)	is received by host
 * 			software. It calls the command handler to execute the command,
 * 			then sends the command handlerTx packet to host software. Finally,
 * 			it returns to the blocked state, waiting for the next packet.
 *
******************************************************************************/

static void vCommandHandlerTask(void *pvParams)
{

	while(1)
	{

		// Task waits for semaphore at this point.
		xSemaphoreTake(xBinarySemaphore, portMAX_DELAY);
		//// !!!!! WAITING !!!!!



		/* Semaphore has been received... continue with task: */
		psGpOutSet(PS_GP_OUT2);	/// TEST SIGNAL: Start of task

		/* === RX FROM HOST === */
		/* Get the data received from the host */
		XUartPs_Recv(&xUartPs1Inst, RxBuffer, UART_RX_BUFFER_SIZE);

		/* Call function to handle the data */
		handleCommand64B(RxBuffer, TxBuffer);

		/* === TX TO HOST === */
		/* Send the response data to the host.
		 * Note that XUartPs_Send() will enable some TX interrupts. */
		XUartPs_Send(&xUartPs1Inst, TxBuffer, UART_TX_BUFFER_SIZE);


		psGpOutClear(PS_GP_OUT2);	/// TEST SIGNAL: End of task
	}
}


/* --- END TASKS ---------------------------------------------------------*/



/*============================================*/
/* =========== INTERRUPT HANDLERS ============*/
/*============================================*/


/*****************************************************************************
 * Function: vUartIntrHandlerForQueue()
 *//**
 *
 * @brief	UART interrupt handler, used in conjunction with the Command Handler
 * 			task. When a (HW) UART interrupt is detected, this ISR is called. It
 * 			sends a semaphore to the command handler task to indicate that the
 * 			64-byte data has been received.
 *
******************************************************************************/

void vUartIntrHandler(void *CallBackRef, uint32_t event, uint32_t event_data)
 {

	BaseType_t xHigherPriorityTaskWoken;
	xHigherPriorityTaskWoken = pdFALSE;


	// --------------------------------------------------------------------------------- //
	// event == XUARTPS_EVENT_RECV_DATA
	// 64 bytes should now have been received from the host.
	// --------------------------------------------------------------------------------- //
	if (event == XUARTPS_EVENT_RECV_DATA)
	{

		psGpOutSet(PS_GP_OUT4);	/// TEST SIGNAL: SET UART RX INTR

		// Dummy delay so that GPIO signal can be clearly seen on scope.
		for (int i = 100; i >=0; i--) {}

		/* Let the command handler know that the data is available. */
		xSemaphoreGiveFromISR(xBinarySemaphore, &xHigherPriorityTaskWoken);
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);

		psGpOutClear(PS_GP_OUT4); /// TEST SIGNAL: CLEAR UART RX INTR

	}


	// --------------------------------------------------------------------------------- //
	// event == XUARTPS_EVENT_SENT_DATA
	// This event should occur when data is sent back to the host.
	// We don't really need to do anything with this event, but we could
	// use it in the future to trigger another task (for example).
	// --------------------------------------------------------------------------------- //
	else if (event == XUARTPS_EVENT_SENT_DATA)
	{

		psGpOutSet(PS_GP_OUT5);		/// TEST SIGNAL: SET UART TX INTR

		// Dummy delay so that GPIO signal can be clearly seen on scope.
		for (int i = 100; i >=0; i--) {}

		psGpOutClear(PS_GP_OUT5);	/// TEST SIGNAL: CLEAR UART TX INTR

	}
 }




/* --- END INTERRUPT HANDLERS ----------------------------------------------------*/




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
	LowLevelInitStatus_s LowLevelInitStatus = { .uart1 = XST_FAILURE,
												.xgpio0 = XST_FAILURE,
												.xgpio1 = XST_FAILURE,
												.xgpiops = XST_FAILURE,
												.xscu_gic = XST_FAILURE
	};


	/* ===== Initialization =====
	 * (1) SCU GIC
	 * (2) PS7 UART0 (also needs reference to GIC to initialise interrupts).
	 * (3) AXI GPIO 0
	 * (4) AXI GPIO 1
	 * (5) AXI PS7 GPIO
	 */

#if PRINT_INIT_STATUS_TO_TERMINAL
	printf("\n\r===== Initializing Drivers =====\n\r");
#endif

	LowLevelInitStatus.xscu_gic = xScuGicInit(&xInterruptController);
	LowLevelInitStatus.uart1 = xUartPs1Init(&xUartPs1Inst, &xInterruptController);
	LowLevelInitStatus.xgpio0 = axiGpio0Init();
	LowLevelInitStatus.xgpio1 = axiGpio1Init();
	LowLevelInitStatus.xgpiops = psGpioInit();



/* Print results to console */
#if PRINT_INIT_STATUS_TO_TERMINAL
	printf("SCUGIC initialization: ");
	if (LowLevelInitStatus.xscu_gic != XST_SUCCESS) 	{ printf("Error detected.\n\r"); }
	else												{ printf("Success.\n\r"); }

	printf("UART0 initialization: ");
	if (LowLevelInitStatus.uart1 != XST_SUCCESS) 		{ printf("Error detected.\n\r"); }
	else												{ printf("Success.\n\r"); }

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

	int init_status = ( (LowLevelInitStatus.xscu_gic == XST_SUCCESS) &&
						(LowLevelInitStatus.uart1 == XST_SUCCESS) &&
						(LowLevelInitStatus.xgpio0 == XST_SUCCESS) &&
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


/*****************************************************************************
 * Function: vApplicationIdleHook( void )
 *//**
 *
 * @brief	Idle Task hook; asserts PS_GP_OUT0 to show when IDLE is running.
 *
******************************************************************************/

void vApplicationIdleHook(void)
{

	psGpOutSet(PS_GP_OUT0); // TEST SIGNAL: Start of Idle Task

	// Dummy delay so that GPIO signal can be clearly seen on scope.
	for (int i = 500; i >=0; i--) {}

	psGpOutClear(PS_GP_OUT0); // TEST SIGNAL: End of Idle Task
}
