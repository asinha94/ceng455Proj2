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
	_queue_id getline_msgq_id = _msgq_open(USER_GET_QUEUE_ID, 0);

	if (_task_get_error() != MQX_EOK) {
		printf("\r\n[%d] failed to create User Message Queue",  _task_get_id());
		printf("\r\nError 0x%x", _task_get_error());
		_task_set_error(MQX_OK);
		_task_block();
	}

#ifdef PEX_USE_RTOS
  while (1) {
#endif
	    char received[MESSAGE_SIZE];

		if (_getline(received)) {
			printf("\r\n[%d] FAILED: Shouldn't have recieved \"%s\" from Handler", _task_get_id(), received);
			received[0] = 0;
		}

		else {
			printf("\r\n[%d] SUCCESS: Getline failed successfully", _task_get_id());
		}

		OpenR(getline_msgq_id);

		if (_getline(received)) {
			printf("\r\n[%d] SUCCESS: Recieved from Getline: %s", _task_get_id(), received);
			received[0] = 0;
		}

		else {
			printf("\r\n[%d] FAILED: Getline should have succeeded...", _task_get_id());
		}

		_queue_id putline_queue_id = OpenW();
		if (putline_queue_id) {

		}

		if(Close()) {
			printf("\r\n[%d] Successfully removed all permissions", _task_get_id());
		}

		else {
			printf("\r\n[%d] Failed to Close() when we should've succeeded...", _task_get_id());
		}




		OSA_TimeDelay(1000);


#ifdef PEX_USE_RTOS
  }
#endif
}



#ifdef __cplusplus
}  /* extern "C" */
#endif


