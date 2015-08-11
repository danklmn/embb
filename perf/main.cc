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

#include <iostream>
#include <fstream>
#include <stdlib.h>

#include <embb/base/c/internal/unused.h>
#include <embb/perf/register.h>
#include <embb/perf/benchmark.h>

#include <embb/containers/lock_free_chromatic_tree.h>
#include <embb/containers/fgl_chromatic_tree.h>
#include <embb/containers/cgl_chromatic_tree.h>

typedef embb::containers::ChromaticTree<int, int>    LockFreeTree;
typedef embb::containers::FGLChromaticTree<int, int> FGLTree;
typedef embb::containers::CGLChromaticTree<int, int> CGLTree;

void Usage(char* prog_name) {
  std::cerr << "USAGE: " << prog_name << " [-T] [-L]" << " [-F | -C]"
      << " [-t<num_threads>]" << " [-s<tree_size>]" << " [-n<num_operations>]"
      << " [-o<output_file>]" << " [-i<insert_rate>]" << " [-d<delete_rate>]"
      << " [-p<prefill_level>]" << " [-a]" << std::endl;
}

int main(int argc, char* argv[]) {
  bool run_latency_test = false;
  bool run_throughput_test = false;
  bool use_fgl_tree = false;
  bool use_cgl_tree = false;
  unsigned int num_threads = 1;
  unsigned int tree_size = 1000;
  unsigned int num_operations = 10;
  std::ofstream dump_stream;
  double insert_rate = 0.25;
  double delete_rate = 0.25;
  double prefill_level = 0.5;
  bool offset_affinity = false;

  for (int i = 1; i < argc; ++i) {
    char opt = 0;
    char *arg = argv[i];

    if ((argv[i][0] == '-' || argv[i][0] == '/') && argv[i][1] != 0) {
      opt = argv[i][1];
      arg = &argv[i][2];
    }
    
    switch (opt) {
    case 'N':
      // Selects the lock-free implementation
      break;
    case 'L':
      run_latency_test = true;
      break;
    case 'T':
      run_throughput_test = true;
      break;
    case 'F':
      use_fgl_tree = true;
      break;
    case 'C':
      use_cgl_tree = true;
      break;
    case 't':
      num_threads = static_cast<unsigned int>(atoi(arg));
      break;
    case 's':
      tree_size = static_cast<unsigned int>(atoi(arg));
      break;
    case 'n':
      num_operations = static_cast<unsigned int>(atoi(arg));
      break;
    case 'o':
      dump_stream.open(arg, std::ios_base::trunc);
      break;
    case 'i':
      insert_rate = atof(arg);
      break;
    case 'd':
      delete_rate = atof(arg);
      break;
    case 'p':
      prefill_level = atof(arg);
      break;
    case 'a':
      offset_affinity = true;
      break;
    default:
      Usage(argv[0]);
      return 1;
    }
  }
  if (!run_latency_test && !run_throughput_test) {
    Usage(argv[0]);
    return 1;
  }


  embb::perf::TreeBenchmark *benchmark = NULL;
  if (use_fgl_tree) {
    benchmark = new embb::perf::TreeBenchmarkImpl<FGLTree>(tree_size,
        insert_rate, delete_rate, prefill_level, offset_affinity);
    std::cout << "Tree type:             Fine-grained Locking\n";
  } else if (use_cgl_tree) {
    benchmark = new embb::perf::TreeBenchmarkImpl<CGLTree>(tree_size,
        insert_rate, delete_rate, prefill_level, offset_affinity);
    std::cout << "Tree type:             Coarse-grained Locking\n";
  } else {
    benchmark = new embb::perf::TreeBenchmarkImpl<LockFreeTree>(tree_size,
        insert_rate, delete_rate, prefill_level, offset_affinity);
    std::cout << "Tree type:             Lock-free\n";
  }

  std::cout << "Number of threads:     " << num_threads << "\n";
  std::cout << "Operations per thread: " << num_operations << "\n";
  std::cout << "Affinity offset:       "
      << (offset_affinity ? "yes" : "no") << "\n";
  std::cout << "Insertion rate:        "
      << static_cast<int>(insert_rate * 100) << "\n";
  std::cout << "Deletion rate:         "
      << static_cast<int>(delete_rate * 100) << "\n";
  std::cout << std::flush;

  unsigned int final_population = 0;

  if (run_latency_test) {
    embb::perf::TreeOpRegister op_register(num_threads, num_operations);
    benchmark->RunLatencyTest(num_threads, num_operations, op_register,
                                           final_population);
    op_register.DumpToStream(dump_stream.is_open() ? dump_stream : std::cout);
  }

  if (run_throughput_test) {
    double throughput;
    benchmark->RunThroughputTest(num_threads, num_operations, throughput,
                                 final_population);
    std::cout << "Throughput: " << throughput << " ops/sec\n";
  }

  std::cout << "Final population: "
      << static_cast<double>(final_population * 100) / tree_size << "%\n";

  delete benchmark;
  
  return 0;
}
