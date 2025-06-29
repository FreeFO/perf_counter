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

#ifndef __PERFC_COMMON_H__
#define __PERFC_COMMON_H__

/*============================ INCLUDES ======================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/
/*!
 * \addtogroup gHelper 4 Helper
 * @{
 */

// for IAR
#if defined(__IAR_SYSTEMS_ICC__)
#   undef __IS_COMPILER_IAR__
#   define __IS_COMPILER_IAR__                  1

// TI Arm Compiler (armcl)
#elif defined(__TI_ARM__)
#   undef __IS_COMPILER_TI_ARM__
#   define __IS_COMPILER_TI_ARM__               1

// TASKING Compiler
#elif defined(__TASKING__)
#   undef __IS_COMPLER_TASKING__
#   define __IS_COMPLER_TASKING__               1

// COSMIC Compiler
#elif defined ( __CSMC__ )
#   undef __IS_COMPILER_COSMIC__
#   define __IS_COMPILER_COSMIC__               1

// for arm compiler 5
#elif ((__ARMCC_VERSION >= 5000000) && (__ARMCC_VERSION < 6000000))
#   undef __IS_COMPILER_ARM_COMPILER_5__
#   define __IS_COMPILER_ARM_COMPILER_5__       1

//for arm compiler 6
#elif defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
#   undef __IS_COMPILER_ARM_COMPILER_6__
#   define __IS_COMPILER_ARM_COMPILER_6__       1

// TI Arm Clang Compiler (tiarmclang)
#elif defined(__ti__)
#undef __IS_COMPILER_TI_ARM_CLANG__
#   define __IS_COMPILER_TI_ARM_CLANG__         1
#else

// for other clang
#   if defined(__clang__) &&                                                    \
    !__IS_COMPILER_ARM_COMPILER_6__ &&                                          \
    !__IS_COMPILER_TI_ARM_CLANG__
#       undef  __IS_COMPILER_LLVM__
#       define __IS_COMPILER_LLVM__                 1

// for gcc
#   elif defined(__GNUC__) && !(  defined(__IS_COMPILER_ARM_COMPILER__)         \
                            ||  defined(__IS_COMPILER_LLVM__)                   \
                            ||  defined(__IS_COMPILER_IAR__))
#       undef __IS_COMPILER_GCC__
#       define __IS_COMPILER_GCC__              1
#   endif

#endif


#undef __IS_COMPILER_ARM_COMPILER__
#if defined(__IS_COMPILER_ARM_COMPILER_5__) && __IS_COMPILER_ARM_COMPILER_5__   \
||  defined(__IS_COMPILER_ARM_COMPILER_6__) && __IS_COMPILER_ARM_COMPILER_6__
#   define __IS_COMPILER_ARM_COMPILER__         1
#endif


#ifndef __PLOOC_VA_NUM_ARGS_IMPL
#   define __PLOOC_VA_NUM_ARGS_IMPL( _0,_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,     \
                                    _12,_13,_14,_15,_16,__N,...)      __N
#endif

#ifndef __PLOOC_VA_NUM_ARGS
#define __PLOOC_VA_NUM_ARGS(...)                                                \
            __PLOOC_VA_NUM_ARGS_IMPL( 0,##__VA_ARGS__,16,15,14,13,12,11,10,9,   \
                                      8,7,6,5,4,3,2,1,0)
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

/*! @} */


#ifdef __cplusplus
}
#endif

#endif
