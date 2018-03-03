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
	printf("\r\n[%d] Starting User Task", _task_get_id());
	_queue_id userq_client_id = _msgq_open(USER_CLIENT_QUEUE_ID, 0);

	if (_task_get_error() != MQX_EOK) {
		printf("\r\n[%d] failed to Handler Message Queue",  _task_get_id());
		printf("\r\nError 0x%x", _task_get_error());
		_task_set_error(MQX_OK);
		_task_block();
	}

	OpenR(userq_client_id);

	OSA_TimeDelay(100);

	char received[MESSAGE_SIZE];
	if (_getline(received)) {
		printf("\r\n[%d] Recieved from Getline: %s", _task_get_id(), received);
	}

	else {
		printf("\r\n[%d] Failed to getline for some reason...", _task_get_id());
	}


	return;

#ifdef PEX_USE_RTOS
  while (1) {
#endif

	  OSA_TimeDelay(100);


#ifdef PEX_USE_RTOS
  }
#endif
}



#ifdef __cplusplus
}  /* extern "C" */
#endif


