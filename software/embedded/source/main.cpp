
extern "C" {

#include "globalVar.h"
#include "stdutil.h"
#include "lcd.h"
#include "servos.h"
}
#include <ch.h>
#include <hal.h>

#include "hardware_config.h"
#include "ttyConsole.h"
#include "ui.h"
#include "StateMachine/StateManager.h"
#include "Dynamixel.h"
#include "STS3032.h"
#include "dshot.h"


static THD_WORKING_AREA(waUiStateMachine, 500);

StateManager stateManager = StateManager();
static void runStateMachine (void *arg)
{
  (void)arg;
  chRegSetThreadName("uiStateMachine");
  stateManager.run();
}


int main(void) {
  /*
   * System initializations.
   * - HAL initialization, this also initializes the configured device drivers
   *   and performs the board-specific initializations.
   * - Kernel initialization, the main() function becomes a thread and the
   *   RTOS is active.
   */
  halInit();
  chSysInit();
  initHeap();

  init_dshot();
  init_servos();
  init_I2C();
  init_CAN();
  init_ui();

  dynamixel.init();
  //sts3032.init() will do the same thing

  stateManager.init();

  chThdCreateStatic(waUiStateMachine, sizeof(waUiStateMachine), NORMALPRIO, &runStateMachine, NULL);

  consoleInit(); // initialisation de la liaison série du shell
  consoleLaunch();  // lancement du thread qui gère le shell sur la liaison série


  // main thread does nothing
  chThdSleep(TIME_INFINITE);
}


