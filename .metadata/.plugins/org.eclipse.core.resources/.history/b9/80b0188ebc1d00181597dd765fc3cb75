/* ###################################################################
**     Filename    : os_tasks.h
**     Project     : proj2
**     Processor   : MK64FN1M0VLL12
**     Component   : Events
**     Version     : Driver 01.00
**     Compiler    : GNU C Compiler
**     Date/Time   : 2018-03-01, 15:46, # CodeGen: 1
**     Abstract    :
**         This is user's event module.
**         Put your event handler code here.
**     Settings    :
**     Contents    :
**         user_task    - void user_task(os_task_param_t task_init_data);
**         handler_task - void handler_task(os_task_param_t task_init_data);
**
** ###################################################################*/
/*!
** @file os_tasks.h
** @version 01.00
** @brief
**         This is user's event module.
**         Put your event handler code here.
*/         
/*!
**  @addtogroup os_tasks_module os_tasks module documentation
**  @{
*/         

#ifndef __os_tasks_H
#define __os_tasks_H
/* MODULE os_tasks */

#include "fsl_device_registers.h"
#include "clockMan1.h"
#include "pin_init.h"
#include "osa1.h"
#include "mqx_ksdk.h"
#include "uart1.h"
#include "fsl_mpu1.h"
#include "fsl_hwtimer1.h"
#include "MainTask.h"
#include "HandlerTask.h"
#include "myUART.h"
#include "UserTask.h"

#ifdef __cplusplus
extern "C" {
#endif 

#include <message.h>

/*Size*/
#define RX_QUEUE_SIZE 1

/*ID*/
#define RX_QUEUE_ID 5
#define RX_QUEUE_HANDLER_ID 6
#define TX_SYS_QUEUE_ID 7



_pool_id RX_MESSAGE_POOL_ID;
_pool_id TX_SYS_MESSAGE_POOL_ID;

typedef struct server_message
{
   MESSAGE_HEADER_STRUCT   HEADER;
   unsigned char DATA;
} SERVER_MESSAGE, * SERVER_MESSAGE_PTR;

typedef struct open_request
{
   MESSAGE_HEADER_STRUCT   HEADER;
   _queue_id queue_id;
   _task_id task_id;
   int exists;
} OPEN_REQUEST, * OPEN_REQUEST_PTR;

/*
** ===================================================================
**     Callback    : user_task
**     Description : Task function entry.
**     Parameters  :
**       task_init_data - OS task parameter
**     Returns : Nothing
** ===================================================================
*/
void user_task(os_task_param_t task_init_data);

/*
** ===================================================================
**     Callback    : handler_task
**     Description : Task function entry.
**     Parameters  :
**       task_init_data - OS task parameter
**     Returns : Nothing
** ===================================================================
*/
void handler_task(os_task_param_t task_init_data);


/* END os_tasks */

#ifdef __cplusplus
}  /* extern "C" */
#endif 

#endif 
/* ifndef __os_tasks_H*/
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
