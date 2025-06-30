/****************************************************************************
*  Copyright 2025 Gorgon Meducer (Email:embedded_zhuoran@hotmail.com)       *
*                                                                           *
*  Licensed under the Apache License, Version 2.0 (the "License");          *
*  you may not use this file except in compliance with the License.         *
*  You may obtain a copy of the License at                                  *
*                                                                           *
*     http://www.apache.org/licenses/LICENSE-2.0                            *
*                                                                           *
*  Unless required by applicable law or agreed to in writing, software      *
*  distributed under the License is distributed on an "AS IS" BASIS,        *
*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. *
*  See the License for the specific language governing permissions and      *
*  limitations under the License.                                           *
*                                                                           *
****************************************************************************/

#ifndef __PERFC_TASK_COMMON_H__
#define __PERFC_TASK_COMMON_H__


/*============================ INCLUDES ======================================*/
#include "perfc_common.h"
#include <setjmp.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>


#ifdef __cplusplus
extern "C" {
#endif

/*!
 * \addtogroup gLanguageExtensios 4 Language Extensions
 * @{
 */

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

#ifndef __FSM_RT_TYPE__
#   define __FSM_RT_TYPE__      1
//! \name finit state machine state
//! @{
typedef enum {
    fsm_rt_err          = -1,    //!< fsm error, error code can be get from other interface
    fsm_rt_cpl          = 0,     //!< fsm complete
    fsm_rt_on_going     = 1,     //!< fsm on-going
    fsm_rt_wait_for_obj = 2,     //!< fsm wait for object
    fsm_rt_asyn         = 3,     //!< fsm asynchronose complete, you can check it later.
    fsm_rt_wait_for_res = 4,     //!< fsm wait for resource
} fsm_rt_t;
//! @}
#endif

typedef union perf_couroutine_rt_t {
    intptr_t *pValue;
    intptr_t  nResult;
    fsm_rt_t  tState;
} perf_couroutine_rt_t;

typedef struct perfc_coroutine_t perfc_coroutine_t;
typedef intptr_t perfc_coroutine_task_handler_t(perfc_coroutine_t *ptThis);

struct perfc_coroutine_t {
    jmp_buf tYieldPoint;
    jmp_buf *ptCaller;
    //perfc_coroutine_task_handler_t *fnHandler;
    perf_couroutine_rt_t tReturn;
};



/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

extern
__attribute__((noinline))
int perfc_coroutine_init(   perfc_coroutine_t *ptTask, 
                            perfc_coroutine_task_handler_t *fnHandler,
                            void *pStackBase,
                            size_t tSizeInByte);

extern
perf_couroutine_rt_t perfc_coroutine_call(perfc_coroutine_t *ptTask);

extern
void perfc_coroutine_yield(perfc_coroutine_t *ptTask);


/*============================ IMPLEMENTATION ================================*/

/*! @} */


#ifdef __cplusplus
}
#endif

#endif
