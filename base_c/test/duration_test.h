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

#ifndef BASE_C_TEST_DURATION_TEST_H_
#define BASE_C_TEST_DURATION_TEST_H_

#include <partest/partest.h>

namespace embb {
namespace base {
namespace test {

/**
 * Provides tests for Base C duration type and methods.
 *
 * These tests access the internal of embb_duration_t, which is necessary to
 * get a starting point without dependencies on not-tested methods. It is
 * discouraged in real usage, due to possible changes to the type.
 */
class DurationTest : public partest::TestCase {
 public:
  /**
   * Adds test methods.
   */
  DurationTest();

 private:
  /**
   * Tests compare method.
   */
  void TestCompare();

  /**
   * Tests the zero duration and setting a duration to zero with all set methods.
   */
  void TestSetZero();

  /**
   * Tests setting and getting values.
   */
  void TestSetAndGet();

  /**
   * Tests adding durations.
   */
  void TestAdd();
};

} // namespace test
} // namespace base
} // namespace embb

#endif  // BASE_C_TEST_DURATION_TEST_H_
