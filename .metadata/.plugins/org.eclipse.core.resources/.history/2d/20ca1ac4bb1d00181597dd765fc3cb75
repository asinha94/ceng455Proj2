/* ###################################################################
**     Filename    : os_tasks.c
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
** @file os_tasks.c
** @version 01.00
** @brief
**         This is user's event module.
**         Put your event handler code here.
*/         
/*!
**  @addtogroup os_tasks_module os_tasks module documentation
**  @{
*/         
/* MODULE os_tasks */

#include "Cpu.h"
#include "Events.h"
#include "rtos_main_task.h"
#include "os_tasks.h"


#ifdef __cplusplus
extern "C" {
#endif 


/* User includes (#include below this line is not maintained by Processor Expert) */

#include "commands.h"

GETLINE_QUEUE getline_q;
MUTEX_STRUCT getline_mutex;

USER_PERMISSIONS user_permissions[NUM_USER_TASK];
MUTEX_STRUCT permissions_mutex;

/*
** ===================================================================
**     Callback    : user_task
**     Description : Task function entry.
**     Parameters  :
**       task_init_data - OS task parameter
**     Returns : Nothing
** ===================================================================
*/
void user_task(os_task_param_t task_init_data)
{
	printf("\r\n[%d] Starting User Task", _task_get_id());
	OpenR(USER_TASK_ID);
  
#ifdef PEX_USE_RTOS
  while (1) {
#endif
    /* Write your code here ... */
	  	char buff[100];
		if(_getline(buff)) {
			printf("\r\n[%d] User Received: %s", _task_get_id(), buff);
		} else {
			printf("\r\n[%d] User Task Doesn't Have read permissions", _task_get_id());
		}

		OSA_TimeDelay(100);

    
#ifdef PEX_USE_RTOS   
  }
#endif    
}

/*
** ===================================================================
**     Callback    : handler_task
**     Description : Task function entry.
**     Parameters  :
**       task_init_data - OS task parameter
**     Returns : Nothing
** ===================================================================
*/
void handler_task(os_task_param_t task_init_data)
{
    getline_q.items_ready = FALSE;

	MUTEX_ATTR_STRUCT mutextattr;
	if (_mutatr_init(&mutextattr) != MQX_EOK) {
		printf("\r\n[%d] Couldn't Init Mutex attr in Serial task", _task_get_id());
	}

	if (permissions_mutex_not_initted) {
		if (_mutex_init(&permissions_mutex, &mutextattr) != MQX_EOK) {
			printf("\r\n[%d] Couldn't Init permissions Mutex in User task", _task_get_id());
		}
		permissions_mutex_not_initted = 0;
	}

	if (getline_mutex_not_initted) {
		if (_mutex_init(&getline_mutex, &mutextattr) != MQX_EOK) {
			printf("\r\n[%d] Couldn't Init getline Mutex in Serial task", _task_get_id());
		}
		getline_mutex_not_initted = 0;
	}


	if (_mutex_lock(&permissions_mutex) != MQX_EOK) {
		printf("\r\n[%d] Couldn't Access Mutex in User task", _task_get_id());
	}

	for (int i = 0; i < NUM_USER_TASK; i++) {
		USER_PERMISSIONS up;
		up.has_read_permissions = FALSE;
		up.has_write_permissions = FALSE;
		up.queue_id = 0;
		up.task_id = USER_TASK_ID;
		user_permissions[i] = up;
	}

	if (_mutex_unlock(&permissions_mutex) != MQX_EOK) {
		printf("\r\n[%d] Couldn't Unlock Mutex in User task", _task_get_id());
	}

	printf("\r\n[%d] Starting Handler Task", _task_get_id());
	// Print Message to console
	char output_message[8];
	sprintf(output_message, "\n\rType: ");
	UART_DRV_SendDataBlocking(myUART_IDX, (uint8_t*)output_message, sizeof(output_message), 1000);


	// Init Rx message queue
	_queue_id rx_queue_id = _msgq_open(RX_QUEUE_ID, 0);

	if (rx_queue_id == 0) {
	 printf("\r\n[%d] Could not open the server message queue\n", _task_get_id());
	 //_task_block();
	}

	_task_set_error(MQX_OK);
	RX_MESSAGE_POOL_ID = _msgpool_create(sizeof(SERVER_MESSAGE), RX_QUEUE_SIZE, 0, 0);

	if (RX_MESSAGE_POOL_ID == MSGPOOL_NULL_POOL_ID) {
	 printf("\r\n[%d] Count not create a message pool: %x\n", _task_get_id(), _task_get_error());
	 //_task_block();
	}

	int size = 100;
	char megaBuffer[size];
	int spacePosition = 0;

	int current_position = 0;
	int last_space = 0;

  
#ifdef PEX_USE_RTOS
  while (1) {
#endif
    /* Write your code here ... */
	  if (_msgq_get_count(rx_queue_id) == 0) {
	  		  continue;
	  }

	  //Get data from the rx queue
	  SERVER_MESSAGE_PTR msg_ptr = _msgq_receive(rx_queue_id, 0);
	  char c =  msg_ptr->DATA;
	  _msg_free(msg_ptr);

	  if (c == '\n' || c == '\r') {
		  if (_mutex_lock(&getline_mutex) != MQX_EOK) {
			  printf("\r\n[%d] Couldn't Access Mutex in Handler", _task_get_id());
		  }

		  memcpy(getline_q.data, megaBuffer, current_position);
		  getline_q.data[current_position] = 0;
		  getline_q.items_ready = TRUE;
		  if (_mutex_unlock(&getline_mutex) != MQX_EOK) {
			  printf("\r\n[%d] Couldn't Unlock Mutex in Handler", _task_get_id());
		  }
		  UART_DRV_SendData(myUART_IDX, (uint8_t*)output_message, sizeof(output_message));
		  //UART_DRV_SendData(myUART_IDX, "\r\n", sizeof("\r\n"));
		  current_position = 0;
		  continue;
	  }

	  if (c == 0x15) { //erase line
		  current_position = 0;
		  char ansi_escape[] =  {0x1B, '[', '2', 'K', '\r'};
		  UART_DRV_SendData(myUART_IDX, (uint8_t*)ansi_escape, sizeof(ansi_escape));
		  continue;
	  }

	  if (c == 0x17) { // erase word
		  spacePosition = 0;
		  int hasSpace = 0;
		  //find last space
		  for(int i = 0; i < current_position; i++){
			  if(megaBuffer[i] == ' '){
				  spacePosition = i;
				  hasSpace = 1;
			  }
		  }

		  if(hasSpace == 1){

			  //Delete a single char x amount of times
			  for(int i = 0; i < (current_position - spacePosition); i++){
				  //printf("delete once\n");
				  //char ansi_escape[] =  {0x1B, '[','1','D'};
				  char ansi_escape[] =  {'\b',' ', '\b'};
				  UART_DRV_SendData(myUART_IDX, (uint8_t*)ansi_escape, sizeof(ansi_escape));
			  }

			  //delete everything to the right of the cursor
			  //char ansi_escape[] = {0x1B, '[', 'K'};
			  //UART_DRV_SendData(myUART_IDX, (uint8_t*)ansi_escape, sizeof(ansi_escape));
		  }
		  //decrement the current position
		  current_position = current_position - (current_position - spacePosition);
		  continue;
	  }

	  if (c == 0x08) { //erase char
		  current_position--;
		  char ansi_escape[] =  {'\b',' ', '\b'};
		  UART_DRV_SendData(myUART_IDX, (uint8_t*)ansi_escape, sizeof(ansi_escape));
		  continue;
	  }

	  //Add char to megaBuffer
	  megaBuffer[current_position] = c;
	  //increment current megaBuffers index
	  current_position++;
	  //Print Mega buffer - hold mega buffer but only print and use for calculations
	  //UART_DRV_SendData(myUART_IDX, (uint8_t*)megaBuffer, current_position);
	  UART_DRV_SendData(myUART_IDX, myRxBuff, sizeof(myRxBuff));
	  //OSA_TimeDelay(10);
    
#ifdef PEX_USE_RTOS   
  }
#endif    
}

/* END os_tasks */

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
