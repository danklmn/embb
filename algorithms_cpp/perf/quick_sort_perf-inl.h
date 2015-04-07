/*
 * Copyright (c) 2014, Siemens AG. All rights reserved.
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

#ifndef EMBB_ALGORITHMS_PERF_QUICK_SORT_PERF_INL_H_
#define EMBB_ALGORITHMS_PERF_QUICK_SORT_PERF_INL_H_

#include <embb/base/perf/call_args.h>
#include <embb/base/memory_allocation.h>
#include <embb/algorithms/quick_sort.h>
#include <embb/tasks/tasks.h>
#include <algorithm>
#include <functional>

namespace embb {
namespace algorithms {
namespace perf {

template<typename T>
SerialQuickSort<T>::SerialQuickSort(const embb::base::perf::CallArgs & args)
: cargs(args), vector_size(args.VectorSize()) {
  v = static_cast<T *>(embb::base::Allocation::AllocateCacheAligned(
    vector_size * sizeof(T)));
  for (size_t i = 0; i < vector_size; i++) {
    v[i] = static_cast<T>(i);
  }
}

template<typename T>
SerialQuickSort<T>::~SerialQuickSort() {
  embb::base::Allocation::FreeAligned(v);
}

template<typename T>
int SerialQuickSort<T>::Greater(const void *lhs, const void *rhs) {
  T l = *reinterpret_cast<const T *>(lhs);
  T r = *reinterpret_cast<const T *>(rhs);
  if (l < r) { return 1; }
  if (l > r) { return -1; }
  return 0;
}

template<typename T>
void SerialQuickSort<T>::Run() {
  std::qsort(v, vector_size, sizeof(T), Greater);
}

template<typename T>
ParallelQuickSort<T>::ParallelQuickSort(const embb::base::perf::CallArgs & args)
: cargs(args), vector_size(args.VectorSize()) {
  v = static_cast<T *>(embb::base::Allocation::AllocateCacheAligned(
    vector_size * sizeof(T)));
  for (size_t i = 0; i < vector_size; i++) {
    v[i] = static_cast<T>(i);
  }
}

template<typename T>
ParallelQuickSort<T>::~ParallelQuickSort() {
  embb::base::Allocation::FreeAligned(v);
}

template<typename T>
void ParallelQuickSort<T>::Run(unsigned int numThreads) {
  // Always reading input values from memory, no CPU-only test possible
  // as quicksort sorts in-place.
  embb::algorithms::QuickSort(
    v, v + vector_size,
    std::greater<T>(),
    embb::tasks::ExecutionPolicy(),
    vector_size / numThreads);
}

} // namespace perf
} // namespace algorithms
} // namespace embb

#endif  // EMBB_ALGORITHMS_PERF_QUICK_SORT_PERF_INL_H_
