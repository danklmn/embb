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

#include <time.h>
#include <random>

#include <embb/base/atomic.h>
#include <embb/base/function.h>
#include <embb/base/thread.h>
#include <embb/base/core_set.h>

#include <embb/perf/timer.h>
#include <embb/perf/register.h>

namespace embb {
namespace perf {

class TreeBenchmark {
 public:
  TreeBenchmark(unsigned int tree_size, double insert_rate,
                double delete_rate, double prefill_level,
                unsigned int affinity_offset, unsigned int affinity_step)
      : tree_size_(tree_size), insert_rate_(insert_rate),
        delete_rate_(delete_rate), prefill_level_(prefill_level),
        operations_per_thread_(0), runners_(),
        sync_barrier_(false), active_threads_(0),
        affinity_offset_(affinity_offset), affinity_step_(affinity_step) {
    srand(static_cast<unsigned int>(time(NULL)));
  }

  virtual void RunLatencyTest(unsigned int threads, unsigned int operations,
                              TreeOpRegister& reg,
                              unsigned int& population) = 0;

  virtual void RunThroughputTest(unsigned int threads, unsigned int operations,
                                 double& throughput,
                                 unsigned int& population) = 0;

  virtual ~TreeBenchmark() {};
  
 protected:
  void InitializeRunners(unsigned int threads) {
    num_threads_ = threads;

    active_threads_ = 0;
    sync_barrier_ = true;

    for (size_t i = 0; i < num_threads_; ++i) {
      embb::base::CoreSet core_set(false);
      core_set.Add((affinity_offset_ + i * affinity_step_) %
                   embb::base::CoreSet::CountAvailable());
      runners_.push_back(new embb::base::Thread(core_set,
          embb::base::MakeFunction(*this, &TreeBenchmark::GenerateLoad)));
    }

    while (active_threads_ < static_cast<int>(num_threads_))
      embb::base::Thread::CurrentYield();
  }

  void StartRunners() {
    sync_barrier_ = false;
  }

  void WaitForRunners() {
    for (size_t i = 0; i < num_threads_; ++i) {
      runners_[i]->Join();
      delete runners_[i];
    }

    runners_.clear();
    num_threads_ = 0;
  }

  virtual void GenerateLoad() = 0;

  TreeBenchmark(const TreeBenchmark&);
  TreeBenchmark& operator=(const TreeBenchmark&);

  const unsigned int tree_size_;
  const double insert_rate_;
  const double delete_rate_;
  const double prefill_level_;
  unsigned int num_threads_;
  unsigned int operations_per_thread_;
  std::vector<embb::base::Thread*> runners_;
  embb::base::Atomic<bool> sync_barrier_;
  embb::base::Atomic<int> active_threads_;
  unsigned int affinity_offset_;
  unsigned int affinity_step_;
};

template<typename Tree>
class TreeBenchmarkImpl : public TreeBenchmark {
 public:
  TreeBenchmarkImpl(unsigned int tree_size, double insert_rate,
                    double delete_rate, double prefill_level,
                    unsigned int affinity_offset, unsigned int affinity_step)
      : TreeBenchmark(tree_size, insert_rate, delete_rate, prefill_level,
                      affinity_offset, affinity_step),
        tree_(NULL) {
  }

  void RunLatencyTest(unsigned int threads, unsigned int operations,
                      TreeOpRegister& reg, unsigned int& population) {
    tree_ = new Tree(tree_size_);
    operations_per_thread_ = operations;

    InitializeRunners(threads);
    
    tree_->reg_ = &reg;
    
    StartRunners();

    WaitForRunners();

    tree_->reg_ = NULL;
    population = TreePopulation();

    delete tree_;
  }

  void RunThroughputTest(unsigned int threads, unsigned int operations,
                         double& throughput, unsigned int& population) {
    tree_ = new Tree(tree_size_);
    operations_per_thread_ = operations;

    InitializeRunners(threads);

    int64_t start_ticks = Timer::Now();
    
    StartRunners();

    WaitForRunners();
    
    double duration = Timer::Since(start_ticks) / 1000000.0;
    
    throughput = threads * operations / duration;

    population = TreePopulation();
    
    delete tree_;
  }

  ~TreeBenchmarkImpl() {}

 private:
  void GenerateLoad() {
    std::random_device rd;
    std::mt19937 key_gen(rd());
    std::mt19937 op_gen(rd());
    std::uniform_int_distribution<> key_dist(0, static_cast<int>(tree_size_));
    std::uniform_real_distribution<> op_dist;

    size_t num_prefill = static_cast<size_t>(tree_size_ * prefill_level_);
    for (size_t i = 0; i < num_prefill / num_threads_; ++i) {
      int key = key_dist(key_gen);
      tree_->TryInsert(key, key);
    }

    ++active_threads_;

    while (sync_barrier_) embb::base::Thread::CurrentYield();

    for (size_t i = 0; i < operations_per_thread_; ++i) {
      int key = key_dist(key_gen);
      double op_rate = op_dist(op_gen);

      if (op_rate <= insert_rate_) {
        tree_->TryInsert(key, key);
      } else if ((op_rate -= insert_rate_) <= delete_rate_) {
        tree_->TryDelete(key);
      } else {
        int value;
        tree_->Get(key, value);
      }
    }

    --active_threads_;
  }

  unsigned int TreePopulation() {
    unsigned int population = 0;

    for (size_t i = 0; i < tree_size_; ++i) {
      int key = static_cast<int>(i);
      int value;
      if (tree_->Get(key, value)) {
        ++population;
      }
    }

    return population;
  }

  Tree* tree_;
};

} // namespace perf
} // namespace embb

#endif // EMBB_PERF_BENCHMARK_H_
