/*
 * UserTasks.c
 *
 *  Created on: Mar 2, 2018
 *      Author: asinha
 */

#include "Cpu.h"
#include "Events.h"
#include "rtos_main_task.h"
#include "os_tasks.h"


#ifdef __cplusplus
extern "C" {
#endif

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
	return;
	printf("\r\n[%d] Starting User Task", _task_get_id());

	_queue_id userq_client_id = _msgq_open(USER_CLIENT_QUEUE_ID, 0);

	if (_task_get_error() != MQX_EOK) {
		printf("\r\n[%d] failed to Handler Message Queue",  _task_get_id());
		printf("\r\nError 0x%x", _task_get_error());
		_task_set_error(MQX_OK);
		_task_block();
	}


	uint16_t user_msg_count = 0;
	USER_REQUEST_PTR msg_ptr;


#ifdef PEX_USE_RTOS
  while (1) {
#endif
	  user_msg_count = _msgq_get_count(userq_client_id);
	  if (user_msg_count == 0 && _task_get_error() != MQX_OK) {
		  printf("\r\nFailed to get user message count.\n");
		  printf("\r\nError code: 0x%x\n", _task_get_error());
		  _task_set_error(MQX_OK);
	  }

	  if (user_msg_count > 0) {
		  msg_ptr = _msgq_receive(userq_client_id, 0);
	      if (_task_get_error() != MQX_EOK) {
			  printf("\r\n[%d] failed to recieve message from handler",  _task_get_id());
			  printf("\r\nError 0x%x", _task_get_error());
			  _task_set_error(MQX_OK);
		  }

	      else {
	    	  printf("\r\nGot \"%s\" from Handler", msg_ptr->DATA);
	    	  _msg_free(msg_ptr);
	      }

	  }
	  OSA_TimeDelay(100);


#ifdef PEX_USE_RTOS
  }
#endif
}



#ifdef __cplusplus
}  /* extern "C" */
#endif

