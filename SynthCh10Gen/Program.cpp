#include "GenerationController.h"


void vUsage(void);

int main(int iArgc, char* aszArgv[]) {
  ControllerStatus result = ControllerStatus::OK;

  GenerationController* controller = new GenerationController();

  result = controller->Init("C:\\atac\\synthetic\\sample config.json");
  if (result != ControllerStatus::OK)
    return 1;

  while (result == ControllerStatus::OK) {
    result = controller->Fire();
  }

  return 0;
}