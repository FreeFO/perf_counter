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

/*============================ INCLUDES ======================================*/
#include "__perfc_task_common.h"
#include "perf_counter.h"
#include "cmsis_compiler.h"
#include <string.h>
#include <stdbool.h>
#include <assert.h>

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
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
extern
__attribute__((noinline))
uintptr_t __perfc_port_get_sp(void);

extern
__attribute__((noinline))
void __perfc_port_set_sp(uintptr_t nSP);

/*============================ IMPLEMENTATION ================================*/

__attribute__((noinline))
int perfc_coroutine_init(   perfc_coroutine_t *ptTask, 
                            perfc_coroutine_task_handler_t *fnHandler,
                            void *pStackBase,
                            size_t tSizeInByte)
{
    do {
        size_t nJmpbufSize = (sizeof(jmp_buf) + 7) & (~((uintptr_t)0x07));
        if (    (NULL == ptTask)
            ||  (NULL == fnHandler)
            ||  (NULL == pStackBase)
            ||  (tSizeInByte < (nJmpbufSize + 17 * sizeof(uint64_t)))) {
            break;
        }
        memset(ptTask, 0, sizeof(*ptTask));
        
        ptTask->pStackBase = pStackBase;
        
        uintptr_t pStackTop = (uintptr_t)pStackBase + tSizeInByte;
        /* force 8bytes alignment */
        pStackTop &= (~((uintptr_t)0x07));
        // add Yieldpoint
        pStackTop -= nJmpbufSize;
        ptTask->ptYieldPoint = (jmp_buf *)pStackTop;

    #if !defined(__PERFC_COROUTINE_NO_STACK_CHECK__)
        // fill the stack with watermark
        do {
            pStackBase = (void *)(((uintptr_t)pStackBase + 7) & (~((uintptr_t)0x07)));
            
            uint32_t * pwStackPointer = (uint32_t *) pStackBase;
            while((uintptr_t)pwStackPointer < pStackTop) {
                *pwStackPointer++ = 0xDEADBEEF;
            }
        } while(0);
    #endif

        typedef volatile struct {
            perfc_coroutine_t *ptTask;
            perfc_coroutine_task_handler_t *fnHandler;
            uintptr_t pnTaskStack;
        } __local_t;
        
        static __local_t s_tWhiteBoard;
        static __local_t * volatile s_ptLocal = &s_tWhiteBoard;

        #define local   (*s_ptLocal)
        
        __IRQ_SAFE {
            local.pnTaskStack = pStackTop;
            local.fnHandler = fnHandler;
            local.ptTask = ptTask;
            jmp_buf tReturnPoint;

            if (setjmp(*(jmp_buf *)&tReturnPoint) == 0) {

                ptTask->ptCaller = &tReturnPoint;
                __perfc_port_set_sp(local.pnTaskStack);
                
                do {
                    /* set the initial yield point */
                    if (0 == setjmp(*(jmp_buf *)(local.ptTask->ptYieldPoint))) {
                        /* the setup phase, return to exit point */
                        longjmp(*(jmp_buf *)(local.ptTask->ptCaller), 1);
                    }
                    local.ptTask->tReturn.nResult = (*local.fnHandler)(local.ptTask);
                } while(1);
            }
        }

        return 0;
    } while(0);

    return -1;
}

__WEAK
__attribute__((noinline))
void perf_coroutine_report_error(   perfc_coroutine_t *ptTask, 
                                    perfc_coroutine_err_t tError)
{
    UNUSED_PARAM(ptTask);
    UNUSED_PARAM(tError);
    assert(tError == PERFC_CR_ERR_NONE);
    while(1) __NOP();
}

size_t perfc_coroutine_stack_remain(perfc_coroutine_t *ptTask)
{
    if (NULL == ptTask) {
        return 0;
    }

    size_t nDWordCount = 0;

#if !defined(__PERFC_COROUTINE_NO_STACK_CHECK__)
    uint64_t *pdwCanary = (uint64_t *)
            (   ((uintptr_t)(ptTask->pStackBase) + 7)
            &   (~((uintptr_t)0x07)));
    
    while(*pdwCanary++ == 0xDEADBEEFDEADBEEFul) {
        nDWordCount++;
    }
#endif

    return nDWordCount * sizeof(uint64_t);

}

perfc_coroutine_rt_t perfc_coroutine_call(perfc_coroutine_t *ptTask)
{
    if (NULL == ptTask) {
        return (perfc_coroutine_rt_t)-1;
    }

    jmp_buf tReturnPoint;
    if (0 == setjmp(tReturnPoint)) {
        ptTask->ptCaller = &tReturnPoint;

        /* go to the yield point */
        longjmp(*(ptTask->ptYieldPoint), 1);
        
        assert(false);
        while(1) __NOP();   /* we should not reach here */
    }

    /* perform stack overflow detection */
#if !defined(__PERFC_COROUTINE_NO_STACK_CHECK__)
    do {
        uint64_t *pdwCanary = (uint64_t *)
            (   ((uintptr_t)(ptTask->pStackBase) + 7)
            &   (~((uintptr_t)0x07)));
        if (*pdwCanary != 0xDEADBEEFDEADBEEFul) {
            /* report stackover flow */
            perf_coroutine_report_error(ptTask, PERFC_CR_ERR_STACK_OVERFLOW);
        }
    } while(0);
#endif

    return ptTask->tReturn;
}

void perfc_coroutine_yield(perfc_coroutine_t *ptTask)
{
    if (NULL == ptTask) {
        return ;
    }

    if (0 == setjmp(*(ptTask->ptYieldPoint))) {
        /* yeild */
        assert(NULL != ptTask->ptCaller);
        longjmp(*(ptTask->ptCaller), 1);
        assert(false);
        while(1) __NOP();   /* we should not reach here */
    }
    /* return from yield */
}


/*! @} */


#ifdef __cplusplus
}
#endif

