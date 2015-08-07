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

#ifndef EMBB_PERF_REGISTER_H_
#define	EMBB_PERF_REGISTER_H_

#include <vector>

#include <embb/base/c/errors.h>
#include <embb/base/c/internal/unused.h>
#include <embb/base/c/internal/thread_index.h>

#include <embb/perf/timer.h>

namespace embb {
namespace perf {

class TreeOpRegister {
 public:
  typedef enum {
    OP_GET,
    OP_INSERT,
    OP_DELETE
  } OpType;

  TreeOpRegister(int threads, int records)
      : num_threads_(threads),
        num_records_(records),
        counters_(num_threads_),
        records_(num_threads_ * num_records_) {}

  void DumpToStream(std::ostream& out) {
    for (int thread = 0; thread < num_threads_; ++thread) {
      out << "# Thread number: " << thread << "\n";
      for (int rec_number = 0; rec_number < num_records_; ++rec_number) {
        Record& record = GetRecord(thread, rec_number);
        out << record.op_type_ << ", "
            << record.full_time_ << ", "
            << record.attempts_ << ", "
            << record.conflicts_ << ", "
            << record.cleanup_time_ << ", "
            << record.cleanup_attempts_ << ", "
            << record.cleanup_conflicts_ << ", "
            << record.search_conflicts_ << ", "
            << record.short_path_ << "\n";
      }
    }
  }

  void StartOperation(OpType type) {
    GetCurrentRecord().op_type_ = type;
    GetCurrentRecord().full_time_ = -Timer::Now();
  }

  void EndOperation() {
    GetCurrentRecord().full_time_ += Timer::Now();
    ++GetCounter(GetThreadNumber());
  }
  
  void Attempt() {
    ++GetCurrentRecord().attempts_;
  }
  
  void Conflict() {
    ++GetCurrentRecord().conflicts_;
  }
  
  void CleanupStart() {
    GetCurrentRecord().cleanup_time_ = -Timer::Now();
  }

  void CleanupEnd() {
    GetCurrentRecord().cleanup_time_ += Timer::Now();
  }

  void CleanupAttempt() {
    ++GetCurrentRecord().cleanup_attempts_;
  }

  void CleanupConflict() {
    ++GetCurrentRecord().cleanup_conflicts_;
  }

  void SearchConflict() {
    ++GetCurrentRecord().search_conflicts_;
  }

  void ShortPathTaken(bool taken) {
    ++GetCurrentRecord().short_path_ = taken;
  }

 private:
  typedef struct {
    OpType  op_type_;
    int64_t full_time_;
    int     attempts_;
    int     conflicts_;
    int64_t cleanup_time_;
    int     cleanup_attempts_;
    int     cleanup_conflicts_;
    int     search_conflicts_;
    bool    short_path_;
  } Record;
  typedef std::vector<int>    CounterVec;
  typedef std::vector<Record> RecordVec;

  Record& GetCurrentRecord() {
    unsigned int thread = GetThreadNumber();
    return GetRecord(thread, GetCounter(thread));
  }

  int& GetCounter(int thread) {
    return counters_[thread];
  }

  Record& GetRecord(int thread, int rec_number) {
    return records_[thread * num_records_ + rec_number];
  }

  unsigned int GetThreadNumber() {
    unsigned int thread;
    int success = embb_internal_thread_index(&thread);
    assert(success == EMBB_SUCCESS); EMBB_UNUSED_IN_RELEASE(success);
    return thread;
  }

  int                 num_threads_;
  int                 num_records_;
  std::vector<int>    counters_;
  std::vector<Record> records_;
};

} // namespace perf
} // namespace embb

#endif // EMBB_PERF_REGISTER_H_
