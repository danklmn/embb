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

#ifndef EMBB_PERF_TIMER_H_
#define	EMBB_PERF_TIMER_H_

#include <assert.h>
#include <stdint.h>

#include <embb/base/c/internal/unused.h>
#include <embb/base/internal/config.h>

#if defined EMBB_PLATFORM_COMPILER_GNUC
#include <time.h>
#elif defined EMBB_PLATFORM_COMPILER_MSVC
#define NOMINMAX
#include <Windows.h>
#else
#error "Unknown compiler"
#endif

namespace embb {
namespace perf {

#if defined EMBB_PLATFORM_COMPILER_MSVC
namespace internal {
static int64_t GetQPCFrequency() {
  LARGE_INTEGER frequency;
  BOOL success = QueryPerformanceFrequency(&frequency);
  assert(success != 0); EMBB_UNUSED_IN_RELEASE(success);
  return frequency.QuadPart;
}
static int64_t QPC_FREQUENCY = GetQPCFrequency();
}
#endif

class Timer {
 public:
  static int64_t Now() {
#if defined EMBB_PLATFORM_COMPILER_GNUC
      struct timespec now;
      int success = clock_gettime(CLOCK_MONOTONIC, &now);
      assert(success == 0); EMBB_UNUSED_IN_RELEASE(success);
      return now.tv_sec * 1000000 + now.tv_nsec / 1000;
#elif defined EMBB_PLATFORM_COMPILER_MSVC
      LARGE_INTEGER now;
      BOOL success = QueryPerformanceCounter(&now);
      assert(success != 0); EMBB_UNUSED_IN_RELEASE(success);
      return (now.QuadPart * 1000000) / internal::QPC_FREQUENCY;
#endif
  }

  static int64_t Since(int64_t start) { return Now() - start; }

  Timer() : start_(Now()) {};

  int64_t Elapsed() { return Now() - start_; }

 private:
  int64_t start_;
};

} // namespace perf
} // namespace embb

#endif // EMBB_PERF_TIMER_H_
