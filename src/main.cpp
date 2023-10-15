#include "composite.h"
#include "creational.h"
#include "solid.h"
#include <CLI/CLI.hpp>
#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <mutex>
#include <vector>

#define kAllCases "all"

/// checks if user wants to execute a particular test case
/// which was set in the first parameter
inline bool canExecute(const std::string &actual, const std::string &needed)
{
  return actual == kAllCases || actual == needed;
}

int main(int argc, const char **argv)
{
  const std::string version = "1.0.0";
  CLI::App app{ "solid principals tester" };
  std::string testcase = kAllCases;
  bool showVersion = false;
  app.add_option("-t,--test-case", testcase, "specific test case to be runned [all]");

  app.add_flag_function(
    "-V,--version",
    [&app, &version](std::int64_t /*flag*/) {
      std::cout << app.get_description() << ", version: " << version << "\n";
      exit(0);
    },
    "shows version of the program and exits");

  CLI11_PARSE(app, argc, argv);

  if (canExecute(testcase, "solid")) { run_solid_examples(); }
  if (canExecute(testcase, "creational")) { run_creational_examples(); }
  if (canExecute(testcase, "composite")) { run_composite_examples(); }
  return 0;
}
