/******************************************************************************
 * @Title		:	Queued Uart Command Handler
 * @Filename	:	queue_uart_cmd_handler.c
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
#include "queue_uart_cmd_handler.h"

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
#define CMDHANDLE_RX_TASK_PRI				2
#define CMDHANDLE_TX_TASK_PRI				2



/* ----------------------- Task 1 --------------------------- */
static void BackgroundTask1( void *pvParameters ); // Toggle LED 1
static TaskHandle_t BackgroundTask1_handle;
/* End Task 1 defs */




/* ------------ Command handler tasks and queues -------------- */

static void vCommandHandlerRxTask( void *pvParams);
static TaskHandle_t rxCommandHandler_h;

static void vCommandHandlerTxTask( void *pvParams);
static TaskHandle_t txCommandHandler_h;

static QueueHandle_t xCmdHandlerRxQueue;
static QueueHandle_t xCmdHandlerTxQueue;


/* Definitions for packets that are sent on the command handler queues. */

#define UART_RX_BUFFER_SIZE			64U		// 64 byte command frame from host
#define UART_TX_BUFFER_SIZE			64U		// 64 byte response frame to host

typedef enum
{
	eUART0 = 0, eUART1,	eCmdHandler
} DataSource_t;

typedef struct
{
	uint8_t 		RxBuffer[UART_RX_BUFFER_SIZE];
	DataSource_t	eDataSource;
} CmdHandlerRxPkt_s;

typedef struct
{
	uint8_t 		TxBuffer[UART_TX_BUFFER_SIZE];
	DataSource_t	eDataSource;
} CmdHandlerTxPkt_s;

// Create the packets:
static CmdHandlerRxPkt_s CmdHandlerUart1RxPkt   = { .RxBuffer = {0}, .eDataSource = eUART1 };
static CmdHandlerTxPkt_s CmdHandlerUart1TxPkt   = { .TxBuffer = {0}, .eDataSource = eCmdHandler };

/* End Command Handler Tasks defs */





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


void vUartIntrHandlerForQueue(void *CallBackRef, uint32_t event, uint32_t event_data);

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
					( const char * ) "Task 1", 			/* Text name for the task, provided to assist debugging only. */
					configMINIMAL_STACK_SIZE, 			/* The stack allocated to the task. */
					NULL, 								/* The task parameter is not used, so set to NULL. */
					tskIDLE_PRIORITY + BACKGROUND_TASK1_PRI,			/* Set Priority. */
					&BackgroundTask1_handle );






	/* Command handler Rx Queue and Task. the task waits on queue data sent by the PS7 UART interrupt handler.
	 * The queue data is of type CmdHandlerUart1RxPkt;  A single-element queue is used, and the packet contains
	 * a 64-byte buffer of data received by the UART. The task has higher priority than Task 1 so that it can
	 * preempt Task 1 to receive UART data as soon as the HW interrupt occurs. But it has lower priority than
	 * the HW timer tasks so that it does not affect the HW-timing. */
	xCmdHandlerRxQueue = xQueueCreate( 1, sizeof(CmdHandlerRxPkt_s) );

	if(xCmdHandlerRxQueue != NULL) {
		xTaskCreate( vCommandHandlerRxTask,
					(const char*) "Command Handler Rx",
					configMINIMAL_STACK_SIZE,
					NULL,
					tskIDLE_PRIORITY + CMDHANDLE_RX_TASK_PRI,
					&rxCommandHandler_h	);
	}



	/* Command handler Tx Queue and Task. The task waits on queue data sent by Timer Task 2 (just defined above).
	 * The queue data is of type CmdHandlerUart1TxPkt;  A single-element queue is used, and the packet contains
	 * a 64-byte buffer of data to be transmitted by the UART back to the host software. The task has higher priority
	 * than Task 1 so that it can preempt Task 1 to send UART data as soon as the packet arrives. But it has lower
	 * priority than the HW timer tasks so that it does not affect the HW-timing. */
	xCmdHandlerTxQueue = xQueueCreate( 1, sizeof(CmdHandlerTxPkt_s) );

	if(xCmdHandlerTxQueue != NULL) {
		xTaskCreate( vCommandHandlerTxTask,
					(const char*) "Command Handler Tx",
					configMINIMAL_STACK_SIZE,
					NULL,
					tskIDLE_PRIORITY + CMDHANDLE_TX_TASK_PRI,
					&txCommandHandler_h	);
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
 * Function: vCommandHandlerRxTask( void *pvParameters )
 *//**
 *
 * @brief	Command handler Receive Task. Waits on a queue packet that is
 * 			generated by the UART interrupt handler when data (64 bytes)
 * 			is received by host software. It calls the command handler
 * 			to execute the command. Then it sends the command handler
 * 			Tx packet to vCommandHandlerTxTask. Finally, it returns to the
 * 			blocked state, waiting for the next packet.
 *
******************************************************************************/

static void vCommandHandlerRxTask(void *pvParams)
{

	while(1)
	{

		/* Task waits for Queue data at this point. */
		xQueueReceive(xCmdHandlerRxQueue, CmdHandlerUart1RxPkt.RxBuffer, portMAX_DELAY);
		//// !!!!! WAITING !!!!!



		/* Queue element has been received... continue with task: */
		psGpOutSet(PS_GP_OUT2);	/// TEST SIGNAL: Start of task

		/* Call function to handle the data */
		handleCommand64B(CmdHandlerUart1RxPkt.RxBuffer, CmdHandlerUart1TxPkt.TxBuffer);

		/* Send the Tx packet to the transmit task. */
		xQueueSend(xCmdHandlerTxQueue, CmdHandlerUart1TxPkt.TxBuffer, portMAX_DELAY);


		psGpOutClear(PS_GP_OUT2);	/// TEST SIGNAL: End of task
	}
}


/*****************************************************************************
 * Function: vCommandHandlerTxTask( void *pvParameters )
 *//**
 *
 * @brief	Command handler Transmit Task. Waits on a queue packet that is
 * 			generated by the Command handler Receive Task. When the packet is
 * 			received, the task calls the UART function to send the data back to
 * 			the host software. It then returns to the blocked state, waiting
 * 			for the next packet.
 *
******************************************************************************/

static void vCommandHandlerTxTask(void *pvParams)
{

	while(1)
	{

		/* Task waits for Queue data at this point. */
		xQueueReceive(xCmdHandlerTxQueue, CmdHandlerUart1TxPkt.TxBuffer, portMAX_DELAY);
		//// !!!!! WAITING !!!!!



		/* Queue element has been received... continue with task: */
		psGpOutSet(PS_GP_OUT3);	/// TEST SIGNAL: Start of task

		/* === TX TO HOST === */
		/* Send the response data to the host.
		 * Note that XUartPs_Send() will enable some TX interrupts. */
		XUartPs_Send(&xUartPs1Inst, CmdHandlerUart1TxPkt.TxBuffer, UART_TX_BUFFER_SIZE);



		psGpOutClear(PS_GP_OUT3);	/// TEST SIGNAL: Start of task
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
 * 			tasks. When a (HW) UART interrupt is detected, this ISR is called. It
 * 			extracts the data from the UART buffer and sends it to vCommandHandlerRxTask.
 *
******************************************************************************/

void vUartIntrHandlerForQueue(void *CallBackRef, uint32_t event, uint32_t event_data)
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


		/* === RX FROM HOST === */
		/* Get the data received from the host */
		XUartPs_Recv(&xUartPs1Inst, CmdHandlerUart1RxPkt.RxBuffer, UART_RX_BUFFER_SIZE);

		/* Send the packet to the Command Handler Rx Task. */
		xQueueSendToFrontFromISR(xCmdHandlerRxQueue, &CmdHandlerUart1RxPkt, &xHigherPriorityTaskWoken);
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
