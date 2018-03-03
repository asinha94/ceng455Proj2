/* ###################################################################
**     Filename    : Events.c
**     Project     : proj2
**     Processor   : MK64FN1M0VLL12
**     Component   : Events
**     Version     : Driver 01.00
**     Compiler    : GNU C Compiler
**     Date/Time   : 2018-03-01, 15:38, # CodeGen: 0
**     Abstract    :
**         This is user's event module.
**         Put your event handler code here.
**     Settings    :
**     Contents    :
**         No public methods
**
** ###################################################################*/
/*!
** @file Events.c
** @version 01.00
** @brief
**         This is user's event module.
**         Put your event handler code here.
*/         
/*!
**  @addtogroup Events_module Events module documentation
**  @{
*/         
/* MODULE Events */

#include "Cpu.h"
#include "Events.h"
#include "rtos_main_task.h"
#include "os_tasks.h"

#ifdef __cplusplus
extern "C" {
#endif 


/* User includes (#include below this line is not maintained by Processor Expert) */

#include <message.h>

/*
** ===================================================================
**     Callback    : myUART_RxCallback
**     Description : This callback occurs when data are received.
**     Parameters  :
**       instance - The UART instance number.
**       uartState - A pointer to the UART driver state structure
**       memory.
**     Returns : Nothing
** ===================================================================
*/

int not_opened = 1;
void myUART_RxCallback(uint32_t instance, void * uartState)
{
    //UART_DRV_SendData(myUART_IDX, myRxBuff, sizeof(myRxBuff));
    _queue_id client_qid;
    if (not_opened) {
    	client_qid = _msgq_open((_queue_number)RX_QUEUE_HANDLER_ID, 0);
		if (client_qid == 0) {
			printf("\nCould not open the client message queue\n");
			_task_block();
		}
		not_opened = 0;
    }

    SERVER_MESSAGE_PTR msg_ptr = (SERVER_MESSAGE_PTR) _msg_alloc(RX_MESSAGE_POOL_ID);

    if (msg_ptr == NULL) {
 	   printf("\nCould not allocate a message\n");
 	   _task_block();
    }

    msg_ptr->HEADER.SOURCE_QID = client_qid;
    msg_ptr->HEADER.TARGET_QID = _msgq_get_id(0, RX_QUEUE_ID);
    msg_ptr->HEADER.SIZE = sizeof(SERVER_MESSAGE);
    msg_ptr->DATA = myRxBuff[0];

    _msgq_send(msg_ptr);
    if (_task_get_error() != MQX_OK) {
 	   printf("\nCould not send a message\n");
 	   return;
    }

    return;
}

/* END Events */

#ifdef __cplusplus
}  /* extern "C" */
#endif 

/*!
** @}
*/
/*
** ###################################################################
**
**     This file was created by Processor Expert 10.5 [05.21]
**     for the Freescale Kinetis series of microcontrollers.
**
** ###################################################################
*/
