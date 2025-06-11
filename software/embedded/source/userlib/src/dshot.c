#include "dshot.h"
#include <ch.h>
#include <hal.h>
#include "stdutil.h"
#include "esc_dshot.h"


DSHOTDriver dshotDriver;
volatile uint16_t _throttle;


static DshotDmaBuffer IN_DMA_SECTION_CLEAR(dshotdDmaBuffer);

static const DSHOTConfig dshotcfg = {
	.dma_stream = STM32_DMA_STREAM_ID(1, 5),
	.dma_channel = 7,
	.pwmp = &PWMD15,
	.tlm_sd = NULL,
	.dma_command = &dshotdDmaBuffer,
};

static THD_WORKING_AREA(waDshotCommand, 2048);
static void dshotCommand(void *);

void init_dshot() {
	chThdCreateStatic(waDshotCommand, sizeof(waDshotCommand), NORMALPRIO, dshotCommand, NULL);
}

void dshot_set_throttle(uint16_t throttle) {
	_throttle = throttle;
}

static void dshotCommand(void *) 
{
  chRegSetThreadName("dshotCommand");
//   while (true)
//   {
// 	chThdSleepMilliseconds(200);
//   }
  
  
  dshotStart(&dshotDriver, &dshotcfg);

  // try to activate extended bidir dshot telemetry
  // during startup sequence
  systime_t now = chVTGetSystemTimeX();
  while(chTimeDiffX(chVTGetSystemTimeX(), now) < TIME_MS2I(400)) {
    dshotSendSpecialCommand(&dshotDriver, DSHOT_ALL_MOTORS, DSHOT_CMD_MOTOR_STOP);
    chThdSleepMicroseconds(500);
#if DSHOT_BIDIR
    dshotSendSpecialCommand(&dshotd, DSHOT_ALL_MOTORS, DSHOT_CMD_BIDIR_EDT_MODE_ON);
    chThdSleepMicroseconds(500);
#endif 
  }
  
  //traceEdt();
  
  
  while (true) {
    now = chVTGetSystemTimeX();

	if(_throttle >= DSHOT_MIN_THROTTLE) {
		dshotSetThrottle(&dshotDriver, DSHOT_ALL_MOTORS, _throttle);
		dshotSendFrame(&dshotDriver);
	} else {
		dshotSendSpecialCommand(&dshotDriver, DSHOT_ALL_MOTORS, DSHOT_CMD_MOTOR_STOP);
	}
	// displayRpm();
	// displayEdt();
	chThdSleepUntilWindowed(now, now + TIME_US2I(500));
  }
}

