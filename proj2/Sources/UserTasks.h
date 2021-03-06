/*
 * UserTasks.h
 *
 *  Created on: Mar 2, 2018
 *      Author: asinha
 */

#ifndef SOURCES_USERTASKS_H_
#define SOURCES_USERTASKS_H_

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

#ifdef __cplusplus
}  /* extern "C" */
#endif

#endif /* SOURCES_USERTASKS_H_ */
