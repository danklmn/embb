#ifndef DATOOLS_DYNAMIC_ANALYSIS_TEST_H_
#define DATOOLS_DYNAMIC_ANALYSIS_TEST_H_

#include <assert.h>
#include <iostream>
#include <vector>
#include <algorithm>

#include <embb/base/thread.h>
#include <embb/base/function.h>
#include <embb/containers/lock_free_chromatic_tree.h>

using embb::base::Thread;
using embb::base::MakeFunction;
typedef embb::containers::ChromaticTree<int, int> LockFreeTree;

class DynamicAnalysisTest {
public:
  DynamicAnalysisTest(int size = 1, int threads = 1,
                      int ops = 0, bool prefill = false)
      : tree_(NULL),
        tree_size_(size),
        num_threads_(threads),
        num_inserts_(ops),
        num_gets_(ops),
        num_deletes_(ops),
        num_max_operations_(ops),
        prefill_(prefill) {}

  int Setup(int argc, char *argv[]) {
    if (ParseArguments(argc, argv)) {
      return -1;
    }
    Thread::SetThreadsMaxCount(num_threads_);
    return 0;
  }

  int CleanUp() {
    return 0;
  }

  int Run() {
    embb_internal_thread_index_reset();

    PrepareTree();

    std::vector<Thread*> threads;
    for (int i = 0; i < num_threads_ - 1; ++i) {
      threads.push_back(new Thread(MakeFunction(*this, &DynamicAnalysisTest::WorkerRoutine), i));
    }

    WorkerRoutine(num_threads_ - 1);

    for (int i = 0; i < num_threads_ - 1; ++i) {
      threads[i]->Join();
      delete threads[i];
    }
    threads.clear();

    ValidateTree();
    
    return 0;
  }

private:
  static const int UNDEFINED_KEY   = -1;
  static const int UNDEFINED_VALUE = UNDEFINED_KEY;
  static const int ILLEGAL_VALUE   = -2;

  int ParseArguments(int argc, char *argv[]) {
    for (int i = 1; i < argc; ++i) {
      char opt = 0;
      char *arg = argv[i];

      if ((argv[i][0] == '-' || argv[i][0] == '/') && argv[i][1] != 0) {
        opt = argv[i][1];
        arg = (argv[i][2] != 0) ? &argv[i][2] : NULL;
      }

      switch (opt) {
      case 's':
        tree_size_ = atoi(arg);
        break; 
      case 't':
        num_threads_ = atoi(arg);
        break;
      case 'i':
        num_inserts_ = atoi(arg);
        break; 
      case 'g':
        num_gets_ = atoi(arg);
        break;
      case 'd':
        num_deletes_ = atoi(arg);
        break;
      case 'p':
        prefill_ = true;
        break;
      default:
        std::cerr << "USAGE: " << argv[0] <<
            " -i<num_inserts>|-g<num_gets>|-d<num_deletes>" <<
            " [-p] [-t<threads>] [-s<size>]";
        return -1;
      }
    }

    num_max_operations_ = std::max(std::max(num_inserts_, num_gets_), num_deletes_);
    tree_size_ = std::max(tree_size_, num_threads_ * num_max_operations_);

    return 0;
  }

  void PrepareTree() {
    tree_ = new LockFreeTree(tree_size_, UNDEFINED_KEY, UNDEFINED_VALUE);

    if (prefill_) {
      for (int i = 0; i < tree_size_; ++i) {
        int key = i;
        int value = key;
        int stored_value = ILLEGAL_VALUE;
        bool success = tree_->TryInsert(key, value, stored_value);
        assert(success);
        assert(stored_value == UNDEFINED_VALUE);
      }
    }
  }

  void WorkerRoutine(int thread_idx) {
    for (int i = 0; i < num_inserts_; ++i) {
      int key = thread_idx * num_max_operations_ + i;
      int value = key;
      int stored_value = ILLEGAL_VALUE;
      bool must_be_stored = prefill_;

      bool success = tree_->TryInsert(key, value, stored_value);

      assert(success);
      assert(stored_value == (must_be_stored ? value : UNDEFINED_VALUE));
    }

    for (int i = 0; i < num_gets_; ++i) {
      int key = thread_idx * num_max_operations_ + i;
      int value = key;
      int stored_value = ILLEGAL_VALUE;
      bool must_be_stored = (prefill_ || i < num_inserts_);

      bool found = tree_->Get(key, stored_value);

      assert(found == must_be_stored);
      assert(must_be_stored ? (stored_value == value) : true);
    }

    for (int i = 0; i < num_deletes_; ++i) {
      int key = thread_idx * num_max_operations_ + i;
      int value = key;
      int stored_value = ILLEGAL_VALUE;
      bool must_be_stored = (prefill_ || i < num_inserts_);

      bool success = tree_->TryDelete(key, stored_value);

      assert(success);
      assert(stored_value == (must_be_stored ? value : UNDEFINED_VALUE));
    }
  }

  void ValidateTree() {
    for (int tidx = 0; tidx < num_threads_; ++tidx) {
      for (int i = 0; i < num_max_operations_; ++i) {
        int key = tidx * num_max_operations_ + i;
        int value = key;
        int stored_value = ILLEGAL_VALUE;
        bool was_inserted = (prefill_ || i < num_inserts_);
        bool must_be_stored = (was_inserted && i >= num_deletes_);

        bool found = tree_->Get(key, stored_value);

        assert(found == must_be_stored);
        assert(must_be_stored ? (stored_value == value) : true);
      }
    }

    delete tree_;
  }

  LockFreeTree *tree_;
  int tree_size_;
  int num_threads_;
  int num_inserts_;
  int num_gets_;
  int num_deletes_;
  int num_max_operations_;
  bool prefill_;
};

#endif // DATOOLS_DYNAMIC_ANALYSIS_TEST_H_
