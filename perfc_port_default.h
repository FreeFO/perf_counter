/****************************************************************************
*  Copyright 2024 Gorgon Meducer (Email:embedded_zhuoran@hotmail.com)       *
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

#ifndef __PERFC_CFG_DISABLE_DEFAULT_SYSTICK_PORTING__
#   define __PERFC_CFG_DISABLE_DEFAULT_SYSTICK_PORTING__    0
#endif

#if !__PERFC_CFG_DISABLE_DEFAULT_SYSTICK_PORTING__

#include "cmsis_compiler.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/

#ifndef __perfc_sync_barrier__
#   define __perfc_sync_barrier__(...)         do {__DSB();__ISB();} while(0)
#endif

#if !defined(__PERFC_NO_STACK_CHECK_IN_ISR__)

#   define __ISR(__ISR_NAME, __STACK_SIZE_HINT)                                 \
volatile                                                                        \
uint32_t g_w##__ISR_NAME##_StackUsage = 0;                                      \
                                                                                \
extern void __origin_##__ISR_NAME (void);                                       \
void __ISR_NAME(void)                                                           \
{                                                                               \
    uint32_t wEXCRETURN;                                                        \
    __ASM volatile ("mov %0, lr"  : "=r" (wEXCRETURN) );                        \
    bool bExtendedStackFrame = !(wEXCRETURN & (1 << 4));                        \
                                                                                \
    uintptr_t nStackLimit = __perfc_port_get_sp() - (__STACK_SIZE_HINT);        \
    __stack_usage_max__(#__ISR_NAME, nStackLimit,                               \
        {                                                                       \
            bExtendedStackFrame                                                 \
                = bExtendedStackFrame                                           \
                &&  !((*(volatile uint32_t *)(0xE000EF34)) & 1<<0);             \
            g_wSysTick_Handler_StackUsage = __stack_used_max__                  \
                                          + 8 * sizeof(uint32_t)                \
                                          + (   bExtendedStackFrame             \
                                            ?   18 * sizeof(uint32_t)           \
                                            : 0);                               \
        }                                                                       \
    ) {                                                                         \
        __origin_##__ISR_NAME();                                                \
    }                                                                           \
}                                                                               \
                                                                                \
void __origin_##__ISR_NAME (void)
#else

#   define __ISR(__ISR_NAME, __STACK_SIZE_HINT)                                 \
volatile                                                                        \
uint32_t g_w##__ISR_NAME##_StackUsage = 0;                                      \
void __ISR_NAME(void)
#endif

#define ISR(__ISR_NAME, __STACK_SIZE_HINT)  __ISR(__ISR_NAME, __STACK_SIZE_HINT)

/*============================ TYPES =========================================*/
typedef uint32_t perfc_global_interrupt_status_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

__STATIC_INLINE 
perfc_global_interrupt_status_t perfc_port_disable_global_interrupt(void)
{
    perfc_global_interrupt_status_t tStatus = __get_PRIMASK();
    __disable_irq();
    
    return tStatus;
}

__STATIC_INLINE 
void perfc_port_resume_global_interrupt(perfc_global_interrupt_status_t tStatus)
{
    __set_PRIMASK(tStatus);
}


#endif
