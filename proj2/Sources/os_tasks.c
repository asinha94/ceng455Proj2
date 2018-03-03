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

#include "Mutex.h"

static MUTEX_STRUCT read_priv_mutex;
static MUTEX_STRUCT write_priv_mutex;

static USER_PRIVILEGE_PTR read_privileges = NULL;
static USER_PRIVILEGE_PTR write_privileges = NULL;

static _queue_id putline_queue_id;

static bool check_rw_privilege(USER_PRIVILEGE_PTR current_priv, _task_id tid) {
	while (current_priv != NULL) {
		if (current_priv->task_id == tid) return TRUE;
		current_priv = current_priv->next;
	}

	return FALSE;
}

bool add_read_privilege(_queue_id qid , _task_id tid) {
	int mut_error = _mutex_lock(&read_priv_mutex);
	if (mut_error != MQX_EOK) {
		printf("\r\n[%d] Couldn't Lock Mutex", _task_get_id());
		printf("\r\nError: %d", mut_error);
		return FALSE;
	}

	USER_PRIVILEGE_PTR rw_head = read_privileges;

	if (check_rw_privilege(rw_head, tid)) return FALSE;

	USER_PRIVILEGE_PTR new_privilege =_mem_alloc(sizeof(USER_PRIVILEGE));
	if (_task_get_error() != MQX_EOK) {
		printf("\r\n[%d] failed to Allocate memory for adding user privilege", _task_get_id());
		printf("\r\nError 0x%x", _task_get_error());
		_task_set_error(MQX_OK);
		return FALSE;
	}

	new_privilege->queue_id = qid;
	new_privilege->task_id = tid;
	new_privilege->next = rw_head;
	read_privileges = new_privilege;
	_mutex_unlock(&read_priv_mutex);
	return TRUE;
}

_queue_id add_write_privilege(_task_id tid) {
	int mut_error = _mutex_lock(&write_priv_mutex);
	if (mut_error != MQX_EOK) {
		printf("\r\n[%d] Couldn't Lock Mutex", _task_get_id());
		printf("\r\nError: %d", mut_error);
		return FALSE;
	}

	USER_PRIVILEGE_PTR rw_head = write_privileges;

	// Check if it alread exists
	if (check_rw_privilege(rw_head, tid)) return FALSE;

	USER_PRIVILEGE_PTR new_privilege =_mem_alloc(sizeof(USER_PRIVILEGE));
	if (_task_get_error() != MQX_EOK) {
		printf("\r\n[%d] failed to Allocate memory for adding user privilege", _task_get_id());
		printf("\r\nError 0x%x", _task_get_error());
		_task_set_error(MQX_OK);
		return FALSE;
	}

	new_privilege->queue_id = 0;
	new_privilege->task_id = tid;
	new_privilege->next = rw_head;

	write_privileges = new_privilege;
	_mutex_unlock(&write_priv_mutex);
	return putline_queue_id;
}

bool remove_read_privilege(_task_id tid) {
	int mut_error = _mutex_lock(&read_priv_mutex);
	if (mut_error != MQX_EOK) {
		printf("\r\n[%d] Couldn't Lock Mutex", _task_get_id());
		printf("\r\nError: %d", mut_error);
		return FALSE;
	}

	USER_PRIVILEGE_PTR rw_head = read_privileges;

	// Check if user has privilege to remove
	if (!check_rw_privilege(rw_head, tid)) return FALSE;

	USER_PRIVILEGE_PTR rw_prev = NULL;
	while (rw_head->task_id != tid) {
		rw_prev = rw_head;
		rw_head = rw_head->next;
	}

	USER_PRIVILEGE_PTR rw_next = rw_head->next;
	// deallocate memory
	_mem_free(rw_head);

	// Check whether removed item was old HEAD
	USER_PRIVILEGE_PTR rw_new_head = rw_next;
	if (rw_prev != NULL) { // RW_NEXT is new HEAD of list
		rw_new_head = rw_prev;
		rw_new_head->next = rw_next;
	}

	read_privileges = rw_new_head;
	return TRUE;
}

bool remove_write_privilege(_task_id tid) {
	int mut_error = _mutex_lock(&write_priv_mutex);
	if (mut_error != MQX_EOK) {
		printf("\r\n[%d] Couldn't Lock Mutex", _task_get_id());
		printf("\r\nError: %d", mut_error);
		return FALSE;
	}

	USER_PRIVILEGE_PTR rw_head = write_privileges;

	// Check if it alread exists
	if (check_rw_privilege(rw_head, tid)) return FALSE;

	USER_PRIVILEGE_PTR rw_prev = NULL;
	while (rw_head->task_id != tid) {
		rw_prev = rw_head;
		rw_head = rw_head->next;
	}

	USER_PRIVILEGE_PTR rw_next = rw_head->next;
	// deallocate memory
	_mem_free(rw_head);

	// Check whether removed item was old HEAD
	USER_PRIVILEGE_PTR rw_new_head = rw_next;
	if (rw_prev != NULL) { // RW_NEXT is new HEAD of list
		rw_new_head = rw_prev;
		rw_new_head->next = rw_next;
	}

	write_privileges = rw_new_head;
	return TRUE;
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
	printf("\r\n[%d] Starting Handler Task", _task_get_id());

	_queue_id userq_server_id = _msgq_open(USER_SERVER_QUEUE_ID, 0);

	if (_task_get_error() != MQX_EOK) {
		printf("\r\n[%d] failed to Handler Message Queue", _task_get_id());
		printf("\r\nError 0x%x", _task_get_error());
		_task_set_error(MQX_OK);
		_task_block();
	}

	user_task_pool_id = _msgpool_create(sizeof(USER_REQUEST), RX_QUEUE_SIZE, 0, 0);
	if (_task_get_error() != MQX_EOK) {
		printf("\r\n[%d] failed to Open Handler Message Pool", _task_get_id());
		printf("\r\nError 0x%x", _task_get_error());
		_task_set_error(MQX_OK);
		_task_block();
	}

	// Delay to allow for the tasks to run
	OSA_TimeDelay(1000);

  
#ifdef PEX_USE_RTOS
  while (1) {
#endif

	  for (int i = 0; i < 10; i++) {
		  printf("\r\n[%d] I am in Handler Task", _task_get_id());
		  USER_REQUEST_PTR msg_ptr = (USER_REQUEST_PTR) _msg_alloc(user_task_pool_id);
		  msg_ptr->HEADER.SOURCE_QID = userq_server_id;
		  msg_ptr->HEADER.TARGET_QID = _msgq_get_id(0, USER_CLIENT_QUEUE_ID);
		  msg_ptr->HEADER.SIZE = sizeof(USER_REQUEST);
		  char tosend[MESSAGE_SIZE];
		  sprintf(tosend, "%d+%d=%d\0", i, i, i+i);
		  strcpy(msg_ptr->DATA, tosend);
		  _msgq_send(msg_ptr);

		  if (_task_get_error() != MQX_EOK) {
			  printf("\r\n[%d] failed to Send Message to User",  _task_get_id());
			  printf("\r\nError 0x%x", _task_get_error());
			  _task_set_error(MQX_OK);
			  _task_block();
			}

		  OSA_TimeDelay(1000);
	  }

	  return;

#ifdef PEX_USE_RTOS   
  }
#endif    
}


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
  /* Write your local variable definition here */
  
#ifdef PEX_USE_RTOS
  while (1) {
#endif
    /* Write your code here ... */
    
    
    OSA_TimeDelay(10);                 /* Example code (for task release) */
   
    
    
    
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