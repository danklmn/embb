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

#include <embb/containers/fgl_chromatic_tree.h>
#include <embb/containers/cgl_chromatic_tree.h>

#include <partest/partest.h>
#include <embb/base/thread.h>

#include "../../containers_cpp/test/tree_test.h"

#define COMMA ,

using embb::containers::test::TreeTest;
using embb::containers::FGLChromaticTree;
using embb::containers::CGLChromaticTree;

PT_MAIN("FGL/CGL Chromatic Trees") {
  unsigned int max_threads = static_cast<unsigned int>(
    2 * partest::TestSuite::GetDefaultNumThreads());
  embb_thread_set_max_count(max_threads);

  PT_RUN(TreeTest< FGLChromaticTree<size_t COMMA int> >);
  PT_RUN(TreeTest< CGLChromaticTree<size_t COMMA int> >);

  PT_EXPECT(embb_get_bytes_allocated() == 0);
}
