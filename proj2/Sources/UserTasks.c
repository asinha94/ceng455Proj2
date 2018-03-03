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
	  	// Call Close before any privileges are given
	    if (!Close()) {
	    	printf("\r\n[%d] SUCCESS. Close() returned False", _task_get_id());
	    }

	    else {
	    	printf("\r\n[%d] FAILED. Close() should've failed", _task_get_id());
	    }

	    char received[MESSAGE_SIZE];

	    // Call getline before before OpenR
		if (_getline(received)) {
			printf("\r\n[%d] FAILED: Shouldn't have recieved \"%s\" from Handler", _task_get_id(), received);
			received[0] = 0;
		}

		else {
			printf("\r\n[%d] SUCCESS: Getline failed successfully", _task_get_id());
		}

		// Call OpenR validly the first time
		if (OpenR(getline_msgq_id)) {
			printf("\r\n[%d] SUCESS: OpenR successfully added Read permissions", _task_get_id());
		}

		else {
			printf("\r\n[%d] FAILED: OpenR should've succeeded", _task_get_id());
		}

		// Call OpenR a second time which should fail
		if (OpenR(getline_msgq_id)) {
			printf("\r\n[%d] FAILED: OpenR should have failed...", _task_get_id());
		}

		else {
			printf("\r\n[%d] SUCESS: OpenR failed correctly", _task_get_id());
		}


		/*
		// Call GetLine which should succeed
		if (_getline(received)) {
			printf("\r\n[%d] SUCCESS: Recieved from Getline: %s", _task_get_id(), received);
			received[0] = 0;
		}

		else {
			printf("\r\n[%d] FAILED: Getline should have succeeded...", _task_get_id());
		}
		*/

		// Call putline before OpenW
		char to_send[] = "Test Putline\0";
		if (_putline(0, to_send)) {
			printf("\r\n[%d] FAILED: Putline should've failed...", _task_get_id());
		}

		else {
			printf("\r\n[%d] SUCCESS: Putline failed correctly",_task_get_id());
		}

		// Call OpenW validly the first time
		_queue_id putline_queue_id = OpenW();
		if (putline_queue_id) {
			printf("\r\n[%d] SUCCESS: OpenW privilege given",_task_get_id());
		}

		else {
			printf("\r\n[%d] FAILED: OpenW failed when it shouldn't have",_task_get_id());
		}

		// Call OpenW when we already have write permission
		_queue_id invalid_putline_queue = OpenW();
		if (invalid_putline_queue) {
			printf("\r\n[%d] FAILED: OpenW should have failed",_task_get_id());
		}

		else {
			printf("\r\n[%d] SUCCESS: OpenW failed correctly",_task_get_id());
		}

		// Call putline after OpenW
		if (_putline(putline_queue_id, to_send)) {
			printf("\r\n[%d] SUCCESS: Putline sent data: %s",_task_get_id(), to_send);
		}

		else {
			printf("\r\n[%d] FAILED: Putline should've worked",_task_get_id());
		}

		// Call GetLine which should succeed with to_send data
		if (_getline(received)) {
			printf("\r\n[%d] SUCCESS: Recieved from Getline: %s", _task_get_id(), received);
			received[0] = 0;
		}

		else {
			printf("\r\n[%d] FAILED: Getline should have succeeded...", _task_get_id());
		}

		if(Close()) {
			printf("\r\n[%d] SUCCESS: removed all permissions", _task_get_id());
		}

		else {
			printf("\r\n[%d] FAILED: to Close() when we should've succeeded...", _task_get_id());
		}

		OSA_TimeDelay(1000);


#ifdef PEX_USE_RTOS
  }
#endif
}



#ifdef __cplusplus
}  /* extern "C" */
#endif


