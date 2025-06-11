#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "esc_dshot.h"

extern DSHOTDriver dshotDriver;

void init_dshot(void);
void dshot_set_throttle(uint16_t throttle);

#ifdef __cplusplus
}
#endif