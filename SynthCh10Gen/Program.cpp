#include "GenerationController.h"



bool StatusOk(ControllerStatus val) {
  if (val == ControllerStatus::OK)
    return true;

  switch (val) {
  case ControllerStatus::INVALID_CONFIG:
    printf("error: Invalid config\n");
    break;
  case ControllerStatus::OPEN_OUTPUT_FILE_FAILED:
    printf("error: Failed to open the output file\n");
    break;
  case ControllerStatus::OPEN_SOURCE_FILE_FAILED:
    printf("error: Failed to open a data source file\n");
    break;
  case ControllerStatus::INVALID_START_TIME:
    printf("error: Invalid start time\n");
    break;
  case ControllerStatus::SOURCES_DEPLETED:
    printf("Done\n");
    break;
  }

  return false;
}


void showHelpString() {
  string helpstring = 
    R"(usage: SynthCh10Gen [options] <config>

  <config>:
    Path to a configuration file that defines data sources and output format.
    See "Configuration Specification.md" for details.

  [options]:
    --help          Show this help string
)";

  printf(helpstring.data());
}


struct CLIParams {
  string configPathname;
} cliParams;

bool processCLI(int argCount, char* args[]) {
  if (argCount != 2)
    return false;

  for (int i = 1; i < argCount; i++) {
    string a = args[i];

    if (a[0] == '-') { // argument is an option
      if (a == "--help" || a == "-h")
        return false; // show help string and exit
    }
    else
    {
      if (i == 1) { // expect configuration pathname
        cliParams.configPathname = a;
      }
    }
  }

  return true;
}


int main(int iArgc, char* aszArgv[]) 
{
  if (!processCLI(iArgc, aszArgv)) {
    showHelpString();
    return 0;
  }

  ControllerStatus result = ControllerStatus::OK;

  GenerationController* controller = new GenerationController();
  result = controller->Init(cliParams.configPathname);
  if (!StatusOk(result))
    return 1;

  while (StatusOk(result)) {
    result = controller->Fire();
  }

  return 0;
}