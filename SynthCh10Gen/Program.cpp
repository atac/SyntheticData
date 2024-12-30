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
  case ControllerStatus::SOURCES_DEPLETED:
    printf("Done\n");
    break;
  }

  return false;
}




int main(int iArgc, char* aszArgv[]) {
  ControllerStatus result = ControllerStatus::OK;

  GenerationController* controller = new GenerationController();

  result = controller->Init("C:\\atac\\synthetic\\configs\\test_config_3.json");
  if (!StatusOk(result))
    return 1;

  while (StatusOk(result)) {
    result = controller->Fire();
  }

  return 0;
}