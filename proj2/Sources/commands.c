/*
 * commands.c
 *
 *  Created on: Mar 1, 2018
 *      Author: asinha
 */

#include "Cpu.h"
#include "rtos_main_task.h"
#include "mutex.h"

#include "commands.h"
#include "os_tasks.h"

_queue_id OpenR(uint16_t queue_id) {
	return add_read_privilege(queue_id, _task_get_id());
}

int _getline(char * received) {
	int queueid = get_read_queueid(_task_get_id());

	if (queueid == 0) { // Dont have permission
		return FALSE;
	}

	int user_msg_count = 0;

	while (1) {
		// checl if we've recieved anything
		  user_msg_count = _msgq_get_count(queueid);
		  if (user_msg_count == 0 && _task_get_error() != MQX_OK) {
			  printf("\r\nFailed to get user message count.\n");
			  printf("\r\nError code: 0x%x\n", _task_get_error());
			  _task_set_error(MQX_OK);
			  // if error when checking, keep iterting and hope we dont get it again
			  continue;
		  }

		  if (user_msg_count > 0) {
			  //if recieved msg then do some error handling
			  USER_REQUEST_PTR msg_ptr = _msgq_receive(queueid, 0);

		      if (_task_get_error() != MQX_EOK) {
				  printf("\r\n[%d] failed to recieve message from handler",  _task_get_id());
				  printf("\r\nError 0x%x", _task_get_error());
				  _task_set_error(MQX_OK);
				  return FALSE;
			  }

			  // copy the buffer into the provided user buffer and free original message
			  strcpy(received, msg_ptr->DATA);
			  _msg_free(msg_ptr);

			  if (_task_get_error() != MQX_EOK) {
				  printf("\r\n[%d] failed to free message",  _task_get_id());
				  printf("\r\nError 0x%x", _task_get_error());
				  _task_set_error(MQX_OK);
				  return FALSE;
			  }

			  return TRUE;
		  }

		  OSA_TimeDelay(100);
	}

	return FALSE; // will never get here but whatever
}

_queue_id OpenW() {
	// the add_write_privilee function just adds the task to the linked list
	// if no record with the same id exists then we return the getline queueid
	// if it does exist we return 0
	return add_write_privilege(_task_get_id());
}

int queue_exists = 0;

int _putline(_queue_id qid, char * string) {
	int queueid = check_write_privilege(_task_get_id());

	if (queueid == 0) { // Dont have permission
		return FALSE;
	}

	// open the putline queue if it doesn't already exist
	_queue_id putline_msgq_id;
	if (!queue_exists) {
		putline_msgq_id = _msgq_open(USER_PUT_QUEUE_ID, 0);
		queue_exists = 1;
	}

	if (_task_get_error() != MQX_EOK) {
		printf("\r\n[%d] failed to create User Message Queue",  _task_get_id());
		printf("\r\nError 0x%x", _task_get_error());
		_task_set_error(MQX_OK);
		return FALSE;
	}

	// create the msg to send along the queue
	USER_REQUEST_PTR msg_ptr = (USER_REQUEST_PTR) _msg_alloc(user_task_pool_id);
	if (msg_ptr == NULL) {
		printf("\r\n[%d] Failed to Allocate Message: Error 0x%x",  _task_get_id(), _task_get_error());
		_task_set_error(MQX_OK);
		return FALSE;
	}

	msg_ptr->HEADER.SIZE = sizeof(USER_REQUEST);
	msg_ptr->HEADER.SOURCE_QID = putline_msgq_id;
	msg_ptr->HEADER.TARGET_QID = qid;
	strcpy(msg_ptr->DATA, string);
	_msgq_send(msg_ptr);

	if (_task_get_error() != MQX_EOK) {
		printf("\r\n[%d] failed to Send Message to User: Error 0x%x",  _task_get_id(), _task_get_error());
		_task_set_error(MQX_OK);
		return FALSE;
	}

	/*
	_msgq_close(putline_msgq_id);
	if (_task_get_error() != MQX_EOK) {
		printf("\r\n[%d] failed to Close message queue: Error 0x%x",  _task_get_id(), _task_get_error());
		_task_set_error(MQX_OK);
		return FALSE;
	}
	*/
	return TRUE;
}

int Close(void) {
	bool read_priv_removed = remove_read_privilege(_task_get_id());
	bool write_priv_removed = remove_write_privilege(_task_get_id());
	// the 2 above functions will return false if no privilege was found
	// so we return false if only both of them are false
	// if one or the other returns true then we return true
	return (read_priv_removed || write_priv_removed);
}



