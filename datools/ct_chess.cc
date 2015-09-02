#include "dynamic_analysis_test.h"

#if defined _WIN32 || defined __CYGWIN__
# ifdef CT_CHESS_DLL
#   define DLL_PUBLIC __declspec(dllexport)
# else
#   define DLL_PUBLIC __declspec(dllimport)
# endif
#else
# define DLL_PUBLIC
#endif

DynamicAnalysisTest *test;

extern "C" {

DLL_PUBLIC int ChessTestStartup(int argc, char *argv[]) {
  test = new DynamicAnalysisTest();
  return test->Setup(argc, argv);
}

DLL_PUBLIC int ChessTestRun(void) {
  return test->Run();
}

DLL_PUBLIC int ChessTestShutdown(void) {
  int res = test->CleanUp();
  delete test;
  return res;
}

}
