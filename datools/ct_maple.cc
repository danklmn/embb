#include "dynamic_analysis_test.h"

int main(int argc, char *argv[]) {
  DynamicAnalysisTest test;

  if (test.Setup(argc, argv) != 0) { return -1; }
  if (test.Run() != 0) { return -1; }
  if (test.CleanUp() != 0) { return -1; }

  return 0;
}
