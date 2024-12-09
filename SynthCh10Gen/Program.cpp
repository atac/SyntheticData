#include "GenerationController.h"


void vUsage(void);

int main(int iArgc, char* aszArgv[]) {
  int result = 0;

  GenerationController* controller = new GenerationController();

  result = controller->Init("");
  if (result != GenerationController::CONTROLLER_OK)
    return result;

  while (result == GenerationController::CONTROLLER_OK) {
    result = controller->Fire();
  }

  return result;
}