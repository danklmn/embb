/*
 * Copyright (c) 2014-2015, Siemens AG. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef EMBB_BASE_INTERNAL_PLATFORM_H_
#define EMBB_BASE_INTERNAL_PLATFORM_H_

/**
 * \file Contains platform-specific includes, typedefs, and defines.
 */

#include <embb/base/internal/config.h>
#include <embb/base/c/internal/platform.h>

#ifdef EMBB_PLATFORM_THREADING_WINTHREADS

namespace embb {
namespace base {
namespace internal {

typedef embb_thread_t ThreadType;
typedef DWORD IDType;
typedef embb_mutex_t MutexType;
typedef embb_condition_t ConditionVariableType;
typedef embb_shared_mutex_t SharedMutexType;

} // namespace internal
} // namespace base
} // namespace embb

#elif defined EMBB_PLATFORM_THREADING_POSIXTHREADS

namespace embb {
namespace base {
namespace internal {

typedef embb_thread_t ThreadType;
typedef embb_thread_id_t IDType;
typedef embb_mutex_t MutexType;
typedef embb_condition_t ConditionVariableType;
typedef embb_shared_mutex_t SharedMutexType;

} // namespace internal
} // namespace base
} // namespace embb

#else // EMBB_PLATFORM_THREADING_POSIXTHREADS

#error "No threading platform defined!"

#endif // else

#endif // EMBB_BASE_INTERNAL_PLATFORM_H_
