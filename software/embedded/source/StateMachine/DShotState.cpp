#include "DShotState.h"
#include "lcd.h"
#include "stdio.h"
#include "stdutil.h"
#include "printf.h"
#include "MainMenuState.h"
#include "dshot.h"

#define DSHOT_MAX 2000

DShotState dshotState = DShotState();


DShotState::DShotState() {
	
}


static int32_t start_enc = 0;

void DShotState::enter(int32_t pos_enc) {
	(void)pos_enc;
	char dshot_txt[] = "DShot           ";
    char blank[] =     "                ";
	char cmd_txt[] = "cmd: 0          ";
	lcdWriteString(&LCDD1, blank, 0);
	lcdWriteString(&LCDD1, blank, 40);
	lcdWriteString(&LCDD1, dshot_txt, 0);
	lcdWriteString(&LCDD1, cmd_txt, 40);
	start_enc = pos_enc;
}

void DShotState::leave() {
    
}

AbstractState* DShotState::onUiEvent(struct UiState ui_state) {
	if(ui_state.ret_clicked) {
        return &mainMenuState;
    }

	if(ui_state.pos_enc < start_enc) {
		start_enc = ui_state.pos_enc;
	}

	uint16_t cmd_speed = ABS(ui_state.pos_enc - start_enc) * 10;

	if(cmd_speed > DSHOT_MAX) {
		cmd_speed = DSHOT_MAX;
		start_enc = ui_state.pos_enc - DSHOT_MAX / 10;
	}

	char txt_cmd[16];
	chsnprintf(txt_cmd, sizeof(txt_cmd), "cmd: %5u", cmd_speed);
	lcdWriteString(&LCDD1, txt_cmd, 40);

	if(cmd_speed != 0) {
		cmd_speed += DSHOT_MIN_THROTTLE-1;
	}

	dshot_set_throttle(cmd_speed);

	return nullptr;
}

AbstractState* DShotState::periodic() {
	return NULL;
}
