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

void Usage(char* prog_name) {
  std::cerr << "USAGE: " << prog_name << " [-T] [-L]" << " [-t<num_threads>]"
      << " [-s<tree_size>]" << " [-n<num_operations>]"
      << " [-o<output_file>]" << std::endl;
}

int main(int argc, char* argv[]) {
  bool run_latency_test = false;
  bool run_throughput_test = false;
  int num_threads = 1;
  int tree_size = 1000;
  int num_operations = 10;
  std::ofstream dump_stream;

  for (int i = 1; i < argc; ++i) {
    char opt = 0;
    char *arg = argv[i];

    if ((argv[i][0] == '-' || argv[i][0] == '/') && argv[i][1] != 0) {
      opt = argv[i][1];
      arg = &argv[i][2];
    }
    
    switch (opt) {
    case 'L':
      run_latency_test = true;
      break;
    case 'T':
      run_throughput_test = true;
      break;
    case 't':
      num_threads = atoi(arg);
      break;
    case 's':
      tree_size = atoi(arg);
      break;
    case 'n':
      num_operations = atoi(arg);
      break;
    case 'o':
      dump_stream.open(arg, std::ios_base::trunc);
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


  embb::perf::TreeBenchmark benchmark(tree_size);

  if (run_latency_test) {
    embb::perf::TreeOpRegister op_register(num_threads, num_operations);
    benchmark.RunLatencyTest(num_threads, num_operations, op_register);
    op_register.DumpToStream(dump_stream.is_open() ? dump_stream : std::cout);

  }

  if (run_throughput_test) {
    double throughput;
    benchmark.RunThroughputTest(num_threads, num_operations, throughput);
    std::cout << "Throughput: " << throughput << "\n";
  }
  
  return 0;
}
