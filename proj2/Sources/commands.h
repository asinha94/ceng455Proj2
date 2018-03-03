/*
 * commands.h
 *
 *  Created on: Mar 1, 2018
 *      Author: asinha
 */

#ifndef SOURCES_COMMANDS_H_
#define SOURCES_COMMANDS_H_

#include "Cpu.h"
#include "Events.h"
#include "rtos_main_task.h"

#include <message.h>

extern _queue_id OpenR(uint16_t);
extern _queue_id OpenW(void);
extern int _getline(char *);
extern int _putline(_queue_id, char *);
extern int Close(void);



#endif /* SOURCES_COMMANDS_H_ */
