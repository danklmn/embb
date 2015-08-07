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

#ifndef EMBB_PERF_BENCHMARK_H_
#define	EMBB_PERF_BENCHMARK_H_

#include <embb/containers/lock_free_chromatic_tree.h>

#include <embb/perf/timer.h>
#include <embb/perf/register.h>

namespace embb {
namespace perf {

class TreeBenchmark {
 public:
  TreeBenchmark(int tree_size, double insert_rate = 0.25,
                double delete_rate = 0.25, double prefill_level = 0.5)
      : tree_size_(tree_size), insert_rate_(insert_rate),
        delete_rate_(delete_rate), prefill_level_(prefill_level),
        tree_(NULL), num_operations_(0), runners_(),
        sync_barrier_(false), active_threads_(0) {
    srand(time(NULL));
  }

  void RunLatencyTest(int threads, int operations, TreeOpRegister& reg) {
    tree_ = new Tree(tree_size_);
    num_operations_ = operations;

    InitializeRunners(threads);
    
    tree_->reg_ = &reg;
    
    StartRunners();

    WaitForRunners();

    delete tree_;
  }

  void RunThroughputTest(int threads, int operations, double& throughput) {
    tree_ = new Tree(tree_size_);
    num_operations_ = operations;

    InitializeRunners(threads);

    int64_t start_ticks = Timer::Now();
    
    StartRunners();

    WaitForRunners();
    
    double duration = Timer::Since(start_ticks) / 1000000.0;
    
    throughput = threads * operations / duration;

    delete tree_;
  }
  
 private:
  typedef embb::containers::ChromaticTree<int, int> Tree;

  void InitializeRunners(int threads) {
    num_threads_ = threads;

    active_threads_ = 0;
    sync_barrier_ = true;

    for (int i = 0; i < num_threads_; ++i) {
      runners_.push_back(new embb::base::Thread(
          embb::base::MakeFunction(*this, GenerateLoad)));
    }

    while (active_threads_ < num_threads_) embb::base::Thread::CurrentYield();
  }

  void StartRunners() {
    sync_barrier_ = false;
  }

  void WaitForRunners() {
    for (int i = 0; i < num_threads_; ++i) {
      runners_[i]->Join();
      delete runners_[i];
    }

    runners_.clear();
    num_threads_ = 0;
  }

  void GenerateLoad() {
    for (int i = tree_size_ * prefill_level_ / num_threads_; i > 0; --i) {
      int key = rand() % tree_size_;
      tree_->TryInsert(key, key);
    }

    ++active_threads_;

    while (sync_barrier_) embb::base::Thread::CurrentYield();

    for (int i = 0; i < num_operations_; ++i) {
      int key = rand() % tree_size_;
      double op_rate = static_cast<double>(rand()) / RAND_MAX;

      if (op_rate < insert_rate_) {
        tree_->TryInsert(key, key);
      } else if ((op_rate -= insert_rate_) < delete_rate_) {
        tree_->TryDelete(key);
      } else {
        int value;
        tree_->Get(key, value);
      }
    }

    --active_threads_;
  }

  TreeBenchmark(const TreeBenchmark&);
  TreeBenchmark& operator=(const TreeBenchmark&);

  const int tree_size_;
  const double insert_rate_;
  const double delete_rate_;
  const double prefill_level_;
  Tree* tree_;
  int num_threads_;
  int num_operations_;
  std::vector<embb::base::Thread*> runners_;
  embb::base::Atomic<bool> sync_barrier_;
  embb::base::Atomic<int> active_threads_;
};

} // namespace perf
} // namespace embb

#endif // EMBB_PERF_BENCHMARK_H_
